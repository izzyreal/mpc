Generated MPC2000XL SND corpus

These fixtures are synthetic and intentionally small.
They are bundled into mpc-tests to exercise the production SND load path
without relying on the handwritten parser as the oracle.

Files:
- mono_corpus_min.SND: Mono corpus case with low behavioral values.
- mono_loop_mid.SND: Mono loop-enabled mid-range case.
- stereo_span.SND: Stereo case that makes non-interleaved payload ordering obvious.
- stereo_max.SND: Stereo case with high behavioral values.
- name_len_sixteen.SND: Exact 16-character name boundary case.
