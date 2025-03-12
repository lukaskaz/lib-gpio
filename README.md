# lib-gpio
Library for controlling gpio line/pins
<br><br>
**Example of the output (w/o and w/ debug logs):**<br>
pi@raspberrypi:\~/git/lib-gpio/build $ ./examples/rpi/native/prog 0<br>
First scenario -> testing read gpio input operation<br>
[INFO] Created input pin: 16<br>
[INFO] Created input pin: 21<br>
Reading first input pin, press switch<br>
[INFO] Input pins monitoring started<br>
[WARN] Pin[21] cannot notify clients, events num: 1<br>
First switch pressed<br>
Reading second input pin, press switch<br>
[WARN] Pin[16] cannot notify clients, events num: 1<br>
[WARN] Pin[16] cannot notify clients, events num: 3<br>
Second switch pressed<br>
Press [enter][WARN] Pin[21] cannot notify clients, events num: 1<br>
<br>
First scenario DONE -> releasing gpio<br>
[INFO] Input pins operation ended<br>
[INFO] Removed input pin: 21<br>
[INFO] Removed input pin: 16<br>
Second scenario -> testing observe gpio input operation<br>
[INFO] Created input pin: 16<br>
[INFO] Created input pin: 21<br>
[INFO] Input pins monitoring started<br>
Press pins, when done press [enter]Pin: 16, val: 1<br>
Pin: 21, val: 3<br>
Pin: 16, val: 2<br>
Pin: 21, val: 1<br>
<br>
Second scenario DONE -> releasing gpio<br>
[INFO] Input pins operation ended<br>
[INFO] Removed input pin: 21<br>
[INFO] Removed input pin: 16<br>
Third scenario -> testing gpio normal output operation<br>
[INFO] Created output pin: 17, type: normal<br>
[INFO] Created output pin: 22, type: normal<br>
[INFO] Output pins operation started<br>
Press [enter]<br>
Press [enter]<br>
Press [enter]<br>
Press [enter]<br>
Third scenario DONE -> releasing gpio<br>
[INFO] Output pins operation ended<br>
[INFO] Removed output pin: 22, type: normal<br>
[INFO] Removed output pin: 17, type: normal<br>
Forth scenario -> testing gpio inverted output operation<br>
[INFO] Created output pin: 17, type: inverted<br>
[INFO] Created output pin: 22, type: inverted<br>
[INFO] Output pins operation started<br>
Press [enter]<br>
Press [enter]<br>
Press [enter]<br>
Press [enter]<br>
Forth scenario DONE -> releasing gpio<br>
[INFO] Output pins operation ended<br>
[INFO] Removed output pin: 22, type: inverted<br>
[INFO] Removed output pin: 17, type: inverted<br>
<br>
pi@raspberrypi:\~/git/lib-gpio/build $ ./examples/rpi/native/prog 1<br>
First scenario -> testing read gpio input operation<br>
[DBG ] Obtained input pin[16] file descriptor 5<br>
[INFO] Created input pin: 16<br>
[DBG ] Obtained input pin[21] file descriptor 6<br>
[INFO] Created input pin: 21<br>
Reading first input pin, press switch<br>
[INFO] Input pins monitoring started<br>
[DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
[WARN] Pin[21] cannot notify clients, events num: 1<br>
First switch pressed<br>
Reading second input pin, press switch<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[WARN] Pin[16] cannot notify clients, events num: 1<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[WARN] Pin[16] cannot notify clients, events num: 2<br>
Second switch pressed<br>
Press [enter][DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
[DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
[DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
[WARN] Pin[21] cannot notify clients, events num: 2<br>
<br>
First scenario DONE -> releasing gpio<br>
[INFO] Input pins operation ended<br>
[INFO] Removed input pin: 21<br>
[INFO] Removed input pin: 16<br>
Second scenario -> testing observe gpio input operation<br>
[DBG ] Obtained input pin[16] file descriptor 5<br>
[INFO] Created input pin: 16<br>
[DBG ] Obtained input pin[21] file descriptor 6<br>
[INFO] Created input pin: 21<br>
[INFO] Input pins monitoring started<br>
[DBG ] Adding observer 0x5556039a54e0 for pin 16<br>
[DBG ] Adding observer 0x5556039a54e0 for pin 21<br>
Press pins, when done press [enter][DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
[DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
Pin: 21, val: 2<br>
[DBG ] Pin[21] clients notified, events num: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
Pin: 16, val: 1<br>
[DBG ] Pin[16] clients notified, events num: 1<br>
[DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
[DBG ] Got interrupt from pin[21] file descriptor 6<br>
[DBG ] Read for pin[21] data size: 16 event size: 16<br>
[DBG ] Read for pin[21] id: 2<br>
Pin: 21, val: 1<br>
[DBG ] Pin[21] clients notified, events num: 1<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
Pin: 16, val: 3<br>
[DBG ] Pin[16] clients notified, events num: 3<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
[DBG ] Got interrupt from pin[16] file descriptor 5<br>
[DBG ] Read for pin[16] data size: 16 event size: 16<br>
[DBG ] Read for pin[16] id: 2<br>
Pin: 16, val: 6<br>
[DBG ] Pin[16] clients notified, events num: 6<br>
<br>
[DBG ] Removing observer 0x5556039a54e0 for pin 16<br>
Second scenario DONE -> releasing gpio<br>
[INFO] Input pins operation ended<br>
[INFO] Removed input pin: 21<br>
[INFO] Removed input pin: 16<br>
Third scenario -> testing gpio normal output operation<br>
[DBG ] Obtained output pin[17] file descriptor 5<br>
[INFO] Created output pin: 17, type: normal<br>
[DBG ] Obtained output pin[22] file descriptor 6<br>
[INFO] Created output pin: 22, type: normal<br>
[INFO] Output pins operation started<br>
[DBG ] Pin[17] written to: 1<br>
[DBG ] Pin[17] set high<br>
[DBG ] Pin[22] written to: 0<br>
[DBG ] Pin[22] set low<br>
Press [enter]<br>
[DBG ] Pin[17] written to: 0<br>
[DBG ] Pin[17] set low<br>
[DBG ] Pin[22] written to: 1<br>
[DBG ] Pin[22] set high<br>
Press [enter]<br>
[DBG ] Pin[17] written to: 1<br>
[DBG ] Pin[17] toggled<br>
[DBG ] Pin[22] written to: 0<br>
[DBG ] Pin[22] toggled<br>
Press [enter]<br>
[DBG ] Pin[17] written to: 0<br>
[DBG ] Pin[17] toggled<br>
[DBG ] Pin[22] written to: 1<br>
[DBG ] Pin[22] toggled<br>
Press [enter]<br>
Third scenario DONE -> releasing gpio<br>
[INFO] Output pins operation ended<br>
[DBG ] Pin[22] written to: 0<br>
[DBG ] Pin[22] set low<br>
[INFO] Removed output pin: 22, type: normal<br>
[DBG ] Pin[17] written to: 0<br>
[DBG ] Pin[17] set low<br>
[INFO] Removed output pin: 17, type: normal<br>
Forth scenario -> testing gpio inverted output operation<br>
[DBG ] Obtained output pin[17] file descriptor 5<br>
[INFO] Created output pin: 17, type: inverted<br>
[DBG ] Obtained output pin[22] file descriptor 6<br>
[INFO] Created output pin: 22, type: inverted<br>
[INFO] Output pins operation started<br>
[DBG ] Pin[17] written to: 0<br>
[DBG ] Pin[17] set high<br>
[DBG ] Pin[22] written to: 1<br>
[DBG ] Pin[22] set low<br>
Press [enter]<br>
[DBG ] Pin[17] written to: 1<br>
[DBG ] Pin[17] set low<br>
[DBG ] Pin[22] written to: 0<br>
[DBG ] Pin[22] set high<br>
Press [enter]<br>
[DBG ] Pin[17] written to: 0<br>
[DBG ] Pin[17] toggled<br>
[DBG ] Pin[22] written to: 1<br>
[DBG ] Pin[22] toggled<br>
Press [enter]<br>
[DBG ] Pin[17] written to: 1<br>
[DBG ] Pin[17] toggled<br>
[DBG ] Pin[22] written to: 0<br>
[DBG ] Pin[22] toggled<br>
Press [enter]<br>
Forth scenario DONE -> releasing gpio<br>
[INFO] Output pins operation ended<br>
[DBG ] Pin[22] written to: 1<br>
[DBG ] Pin[22] set low<br>
[INFO] Removed output pin: 22, type: inverted<br>
[DBG ] Pin[17] written to: 1<br>
[DBG ] Pin[17] set low<br>
[INFO] Removed output pin: 17, type: inverted<br>
