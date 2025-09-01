#include "engine.h"

int main(int /*argc*/, const char** /*argv*/)
{
    RealmEngine::Engine* engine = new RealmEngine::Engine();

    engine->boot();
    engine->run();
    engine->terminate();

    return 0;
}