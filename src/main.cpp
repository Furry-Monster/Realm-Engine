#include "engine.h"

int main(int /*argc*/, const char** /*argv*/)
{
    Engine* engine = new Engine();
    engine->boot();
    engine->run();
    engine->terminate();
    return 0;
}