# Kaitai Parser Maintenance

This repo vendors generated C++ Kaitai parsers under:

- `src/main/file/kaitai/generated`

The source `.ksy` files live in:

- `/Users/izmar/git/mpc2000xl_kaitai`

The generator we use is the C++ write-capable compiler from:

- `/Users/izmar/git/kaitai_struct`
- branch: `cpp-serialization`

## Formats currently generated

- `mpc2000xl.snd.ksy`
- `mpc2000xl.pgm.ksy`
- `mpc2000xl.aps.ksy`
- `standard_midi_file_with_running_status.ksy`
- `mpc2000xl.mid.ksy`
- `mpc2000xl.all.ksy`
- `mpc60.set.v1.ksy`

## Regenerate parsers

From anywhere:

```sh
/Users/izmar/git/kaitai_struct/compiler/jvm/target/universal/stage/bin/kaitai-struct-compiler \
  -t cpp_stl \
  --cpp-standard 11 \
  -w \
  --outdir /Users/izmar/git/vmpc-juce/editables/mpc/src/main/file/kaitai/generated \
  -I /Users/izmar/git/mpc2000xl_kaitai \
  /Users/izmar/git/mpc2000xl_kaitai/mpc2000xl.snd.ksy \
  /Users/izmar/git/mpc2000xl_kaitai/mpc2000xl.pgm.ksy \
  /Users/izmar/git/mpc2000xl_kaitai/mpc2000xl.aps.ksy \
  /Users/izmar/git/mpc2000xl_kaitai/standard_midi_file_with_running_status.ksy \
  /Users/izmar/git/mpc2000xl_kaitai/mpc2000xl.mid.ksy \
  /Users/izmar/git/mpc2000xl_kaitai/mpc2000xl.all.ksy \
  /Users/izmar/git/mpc2000xl_kaitai/mpc60.set.v1.ksy
```

## Typical update flow

1. Change the relevant `.ksy` file in `mpc2000xl_kaitai`.
2. Regenerate the C++ outputs into `src/main/file/kaitai/generated`.
3. If needed, update the integration layer in:
   - `src/main/file/kaitai/SndIo.*`
   - `src/main/file/kaitai/PgmIo.*`
   - `src/main/file/kaitai/ApsIo.*`
   - `src/main/file/kaitai/MidIo.*`
   - `src/main/file/kaitai/AllIo.*`
4. Run downstream tests.
5. Commit both the `.ksy` change and the regenerated vendored C++.

## Verify in editables/mpc

Build tests. Use any build directory you want. Examples below use `build`:

```sh
cmake -G "Ninja Multi-Config" -B build
cmake --build build --target mpc-tests --config Debug
```

Run the focused Kaitai suite:

```sh
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-snd],[kaitai-pgm],[kaitai-aps],[kaitai-mid],[kaitai-all],[kaitai-set]"
```

Run one format only if needed:

```sh
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-aps]"
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-pgm]"
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-snd]"
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-mid]"
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-all]"
./build/Debug/mpc-tests.app/Contents/MacOS/mpc-tests "[kaitai-set]"
```

## Notes

- Do not regenerate with upstream Kaitai compiler unless it has the same C++ serialization support as the `cpp-serialization` branch.
- `generated/` is vendored on purpose. As far as `editables/mpc` is concerned, generated parsers are ordinary source files.
