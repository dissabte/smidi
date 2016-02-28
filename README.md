# smidi
MIDI library with MIDI Clock synchronization support.<br>
You can generate doxygen documentation:<br>

~~~bash
# assuming you are in a build folder located next to smidi repository:
cmake ../smidi/smidi/ && cmake --build . --target smidi_docs
~~~

And in case you want to look at internal documentation (i.e. the one for library private parts):<br>

~~~bash
# assuming you are in a build folder located next to smidi repository:
cmake -DDOXYGEN_SECTIONS=INTERNAL ../smidi/smidi/ && cmake --build . --target smidi_docs
~~~

# Still in development...
- [x] demo app uses [watcher](https://github.com/dissabte/watcher) to take hot plugged MIDI devices into account.
- [ ] MIDI sync is not even started!
- [ ] OS X (CoreMidi)? Some day.
- [ ] Windows (winmm)? Some day.
