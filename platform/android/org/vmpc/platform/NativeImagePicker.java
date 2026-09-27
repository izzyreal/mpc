package org.vmpc.platform;

import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.provider.OpenableColumns;
import java.lang.ref.WeakReference;

/** Host glue: attach the Activity before constructing MPC; no picker opens at attach time. */
public final class NativeImagePicker {
    private static WeakReference<Activity> activity = new WeakReference<>(null);
    private static Context context;
    private static native void initialize();
    private static native void complete(long request, String uri, String label, String error);

    public static void attach(Activity value) {
        context = value.getApplicationContext();
        activity = new WeakReference<>(value);
        initialize();
    }
    public static void detach(Activity value) {
        if (activity.get() == value) activity.clear();
    }
    public static int openImage(String location, boolean readOnly) throws Exception {
        if (context == null) throw new IllegalStateException("Missing application context");
        try (ParcelFileDescriptor file = context.getContentResolver().openFileDescriptor(Uri.parse(location), readOnly ? "r" : "rw")) {
            if (file == null) throw new IllegalStateException("Provider returned no image");
            return file.detachFd();
        }
    }
    public static void pick(long request) {
        final Activity host = activity.get();
        if (host == null || host.isFinishing()) {
            complete(request, null, null, "Image picker needs an active Activity");
            return;
        }
        host.runOnUiThread(() -> {
            try {
                Picker fragment = new Picker();
                Bundle args = new Bundle();
                args.putLong("request", request);
                fragment.setArguments(args);
                host.getFragmentManager().beginTransaction().add(fragment, "mpc-image-" + request).commit();
            } catch (Exception e) { complete(request, null, null, "Cannot present image picker"); }
        });
    }
    public static void cancel(long request) {
        final Activity host = activity.get();
        if (host == null) return;
        host.runOnUiThread(() -> {
            Fragment fragment = host.getFragmentManager().findFragmentByTag("mpc-image-" + request);
            if (fragment != null) {
                // Android owns the external document picker. Removing its
                // receiver makes a later result harmless after core cancellation.
                host.getFragmentManager().beginTransaction().remove(fragment).commitAllowingStateLoss();
            }
        });
    }
    @SuppressWarnings("deprecation")
    public static final class Picker extends Fragment {
        static final int REQUEST_CODE = 17236;
        private long request;
        private boolean delivered;
        @Override public void onCreate(Bundle saved) {
            super.onCreate(saved);
            request = getArguments().getLong("request");
            delivered = saved != null && saved.getBoolean("delivered");
            if (saved != null) return;
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
            try { startActivityForResult(intent, REQUEST_CODE); }
            catch (Exception e) { finish(null, null, "Native image picker unavailable"); }
        }
        @Override public void onSaveInstanceState(Bundle state) {
            state.putBoolean("delivered", delivered);
            super.onSaveInstanceState(state);
        }
        private void finish(String uri, String label, String error) {
            if (delivered) return;
            delivered = true;
            complete(request, uri, label, error);
            if (getFragmentManager() != null)
                getFragmentManager().beginTransaction().remove(this).commitAllowingStateLoss();
        }
        @Override public void onActivityResult(int code, int result, Intent data) {
            if (code != REQUEST_CODE) { super.onActivityResult(code, result, data); return; }
            if (result != Activity.RESULT_OK || data == null || data.getData() == null) { finish(null, null, null); return; }
            final Uri uri = data.getData();
            try {
                int flags = data.getFlags() & (Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
                context.getContentResolver().takePersistableUriPermission(uri, flags);
                String label = uri.getLastPathSegment();
                try (Cursor cursor = context.getContentResolver().query(uri, new String[]{OpenableColumns.DISPLAY_NAME}, null, null, null)) {
                    if (cursor != null && cursor.moveToFirst()) label = cursor.getString(0);
                }
                finish(uri.toString(), label, null);
            } catch (Exception e) { finish(null, null, "Provider cannot retain direct image access"); }
        }
        @Override public void onDestroy() {
            Activity host = getActivity();
            if (!delivered && (host == null || !host.isChangingConfigurations())) {
                delivered = true;
                complete(request, null, null, null);
            }
            super.onDestroy();
        }
    }
}
