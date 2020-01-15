#ifndef DEGUG_LOGGING
#define DEGUG_LOGGING

namespace Engine::Debug {
    class Logger {

    private:
        Logger();

    public:

        enum class Severity {
            Info,
            Warning,
            Error
        };

    };
}

#define LOGGER std::cerr
#endif // !DEGUG_LOGGING
