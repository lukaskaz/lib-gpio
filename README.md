# lib-gpio
Library for controlling gpio line/pins
<br><br>
**Example of the output (w/o and w/ debug logs):**<br>
pi@raspberrypi:\~/git/lib-gpio/build $ ./examples/rpi/native/prog 0<br>
First scenario -> testing gpio input operation<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 16<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)::<lambda()>] Input pins monitoring started<br>
Pin: 21, val: 2<br>
Pin: 16, val: 4<br>
Pin: 21, val: 1<br>
<br>
First scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Input pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 16<br>
Second scenario -> testing gpio output operation<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 17<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 22<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)] Output pins operation started<br>
Second scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Output pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 22<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 17<br>
<br>
pi@raspberrypi:\~/git/lib-gpio/build $ ./examples/rpi/native/prog 1<br>
First scenario -> testing gpio input operation<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::InputPin::initialize()] Obtained input pin[16] file descriptor 4<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 16<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::InputPin::initialize()] Obtained input pin[21] file descriptor 5<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::InputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created input pin: 21<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::read(int32_t, std::shared_ptr<Observer<std::pair<int, int> > >)] Adding observer 0x555595eb6d70 for pin 16<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::read(int32_t, std::shared_ptr<Observer<std::pair<int, int> > >)] Adding observer 0x555595eb6d70 for pin 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)::<lambda()>] Input pins monitoring started<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::unread(int32_t, std::shared_ptr<Observer<std::pair<int, int> > >)] Removing observer 0x555595eb6d70 for pin 16<br>
First scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Input pins operation ended<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 21<br>
[INFO][gpio::rpi::native::Gpio::Handler::InputPin::~InputPin()] Removed input pin: 16<br>
Second scenario -> testing gpio output operation<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::initialize()] Obtained output pin[17] file descriptor 6<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 17<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::initialize()] Obtained output pin[22] file descriptor 7<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::OutputPin(gpio::rpi::native::Gpio::Handler*, int32_t)] Created output pin: 22<br>
[INFO][gpio::rpi::native::Gpio::Handler::Handler(const gpio::rpi::native::config_t&)] Output pins operation started<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set()] Pin[17] set<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear()] Pin[22] cleared<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear()] Pin[17] cleared<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set()] Pin[22] set<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set()] Pin[17] set<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle()] Pin[17] toggled<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear()] Pin[22] cleared<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle()] Pin[22] toggled<br>
<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear()] Pin[17] cleared<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle()] Pin[17] toggled<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::set()] Pin[22] set<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::toggle()] Pin[22] toggled<br>
<br>
Second scenario DONE -> releasing gpio<br>
[INFO][gpio::rpi::native::Gpio::Handler::~Handler()] Output pins operation ended<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear()] Pin[22] cleared<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 22<br>
[DBG][bool gpio::rpi::native::Gpio::Handler::OutputPin::clear()] Pin[17] cleared<br>
[INFO][gpio::rpi::native::Gpio::Handler::OutputPin::~OutputPin()] Removed output pin: 17<br>
