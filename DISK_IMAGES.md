# Disk images

DISKS can bind an existing image to a virtual device. The first iteration uses
exactly the Akai FAT filesystem implementation used for raw USB volumes. It
accepts a single FAT16/Akai FAT16 volume starting at byte zero with 512-byte
sectors. FAT12, FAT32, partition tables, ISO9660, compressed images, and other
container formats are not supported. File extensions do not determine support.

In DISKS, press **IMAGES** (F4), or WINDOW, for ADD, REPLACE, CHECK, REMOVE, and
BACK. ADD uses the operating system's native file picker. New bindings default
to read-only. BACK returns to the volume list; the wheel changes the access
mode and SAVE commits it. Save pending mode edits before entering IMAGES.

Selecting the device in LOAD/SAVE opens its filesystem. Switching away flushes
and releases it. Changing the active device's access mode also releases its
existing access and selects an enabled fallback device; reselect it to use the
new mode. The last active image is reopened when disks initialize on the next
launch. If it cannot be opened, DEFAULT is used and the binding is retained.

CHECK opens inactive images temporarily in read-only mode. For an active image,
it checks the current mounted session. To replace or remove an active binding,
first select another device. Removing a binding never removes the source file.

Read/write means writing directly to the selected original image. MPC does not
import, copy, extract, truncate, create, or grow image files. Mobile document
providers must offer a seekable file descriptor and persistent access; otherwise
the operation reports an error. A read/write request is never silently changed
to read-only. Cooperative file locks prevent conflicting mounted access; other
software must also respect file locking.

## Consuming the core

Existing consumers require no new initialization when they do not use the
picker. Programmatic binding and mounting of accessible local images require no
presentation context on any platform. `DiskController::bindImage`,
`validateImage`, `removeImage`, `setVolumeMode`, and `activateDisk` return an empty
string on success or an error otherwise. `platform::selectedImage` creates a
binding with a fresh identity. Binding validates without keeping the image open.
`MpcInitOptions::imageOpen` remains an optional access-provider injection point.

`DiskController::pickImage` starts asynchronous native selection. Completion is
consumed on the core owner thread by the existing `LayeredScreen::timerCallback`.
A consumer with its own event loop may instead call `pollFilePicker` there. Do
not invoke these APIs from an audio callback. Destroying the controller cancels
pending selection and prevents late results from modifying the core.

Desktop native parent registration is optional:

- Windows accepts a null parent, or an HWND via `setFilePickerParent`.
- macOS accepts a null parent, or an NSWindow pointer. AppKit presentation runs
  on the main thread and requires an application/main event loop. A registered
  parent gives a sheet; without one the panel appears independently.
- Linux uses the XDG FileChooser portal. No parent is necessary. An optional
  portal identifier can be passed as the second argument, e.g. `x11:<XID>` or
  `wayland:<exported handle>`. A desktop session bus and an installed portal
  backend implementing FileChooser are required. The build uses GIO, already
  part of the Linux USB-volume dependency stack.

If registering a parent, call `setFilePickerParent(nullptr)` before destroying
it. Changing/clearing a registered parent cancels its outstanding request. No
registration or teardown notification is required for unparented desktop use.

## Mobile integration

All picker, document reference, and image access code belongs to this repository;
there is no JUCE or SDL dependency.

On iOS, register the presenting UIView with `setFilePickerParent` when its host
view is available, and clear it before the view is destroyed. The adapter locates
the owning view controller and uses document-open mode, not import/copy mode.
Security-scoped bookmark data is stored with the binding and used on reopen.
The picker reports an error if there is no presenting view. App-owned files can
still be bound programmatically without a view or bookmark.

On Android, include `platform/android` in the application's Java source set.
After the native library loads and before constructing MPC, call
`org.vmpc.platform.NativeImagePicker.attach(activity)`. Call `detach(activity)`
when that Activity is destroyed and attach its replacement after recreation.
The helper owns result routing through a Fragment, persists the actual granted
URI permissions, and opens the original document with `r` or `rw` access.
Regular accessible filesystem paths do not need this bridge. Document URIs need
the initialized application context even when reopening without a picker.

## Persistence and testing

`volumes.json` remains compatible with existing USB entries. Image records add
`type: "image"`, `path`, `label`, `size`, and an opaque `access` token alongside
UUID, mode, and active state. Configuration updates replace a temporary file;
failed binding persistence rolls back the in-memory addition/replacement.

The `[image]` tests cover deferred mounting, direct writes and reopening,
read-only byte preservation, access conflicts, unavailable images, geometry and
allocation-chain validation, persistence, startup restoration without a UI
context, and DISKS scrolling. Native presentation needs platform UI smoke tests
in addition to these hardware-independent tests.
