# lib-gpio
Library for controlling gpio line/pins
<br><br>
**Example of the output (w/o and w/ debug logs):**<br>
pi@raspberrypi:\~/git/lib-gpio/build $ ./examples/rpi/native/prog 0<br>
First scenario -> testing gpio input operation<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 16<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)::<lambda()>] Input pins monitoring started<br>
Pin: 16, val: 5<br>
Pin: 21, val: 3<br>
Pin: 16, val: 1<br>
<br>
First scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Input pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 16<br>
<br>
Second scenario -> testing gpio normal output operation<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 17, type: normal<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 22, type: normal<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)] Output pins operation started<br>
<br>
Second scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Output pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 22, type: normal<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 17, type: normal<br>
<br>
Third scenario -> testing gpio inverted output operation<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 17, type: inverted<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 22, type: inverted<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)] Output pins operation started<br>
<br>
Third scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Output pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 22, type: inverted<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 17, type: inverted<br>
<br>
pi@raspberrypi:\~/git/lib-gpio/build $ ./examples/rpi/native/prog 1<br>
First scenario -> testing gpio input operation<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::InputPin::initialize()] Obtained input pin[16] file descriptor 4<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 16<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::InputPin::initialize()] Obtained input pin[21] file descriptor 5<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 21<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::read(int32_t, std::shared_ptr<Observer<std::pair<int, int> > >)] Adding observer 0x5555cdf7ad70 for pin 16<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::read(int32_t, std::shared_ptr<Observer<std::pair<int, int> > >)] Adding observer 0x5555cdf7ad70 for pin 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)::<lambda()>] Input pins monitoring started<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[21] file descriptor 5<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[21] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[21] id: 2<br>
Pin: 21, val: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::InputPin::notify()] Pin[21] clients notified, events num: 1<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Got interrupt from pin[16] file descriptor 4<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] data size: 16 event size: 16<br>
[DBG][int32_t gpio::rpi::native::Gpio::Handler::InputPin::waitforevent(int32_t) const] Read for pin[16] id: 2<br>
Pin: 16, val: 3<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::InputPin::notify()] Pin[16] clients notified, events num: 3<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::unread(int32_t, std::shared_ptr<Observer<std::pair<int, int> > >)] Removing observer 0x5555cdf7ad70 for pin 16<br>
<br>
First scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Input pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 16<br>
<br>
Second scenario -> testing gpio normal output operation<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::initialize()] Obtained output pin[17] file descriptor 4<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 17, type: normal<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::initialize()] Obtained output pin[22] file descriptor 5<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 22, type: normal<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)] Output pins operation started<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set() const] Pin[17] set high<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[22] set low<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[17] set low<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set() const] Pin[22] set high<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[17] toggled<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[22] toggled<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[17] toggled<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[22] toggled<br>
<br>
Second scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Output pins operation ended<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[22] set low<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 22, type: normal<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[17] set low<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 17, type: normal<br>
<br>
Third scenario -> testing gpio inverted output operation<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::initialize()] Obtained output pin[17] file descriptor 4<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 17, type: inverted<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::initialize()] Obtained output pin[22] file descriptor 5<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 22, type: inverted<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)] Output pins operation started<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set() const] Pin[17] set high<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[22] set low<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[17] set low<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set() const] Pin[22] set high<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[17] toggled<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[22] toggled<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[17] toggled<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 0<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle() const] Pin[22] toggled<br>
<br>
Third scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Output pins operation ended<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[22] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[22] set low<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 22, type: inverted<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::write(uint8_t) const] Pin[17] written to: 1<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear() const] Pin[17] set low<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 17, type: inverted<br>
