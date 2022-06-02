#include <pirate/tr/bar/BarSimRunner.hpp>
using pirate::tr::bar::BarSimRunner;

#include <NVecho-factory.hpp>

int main(int argc, char** argv) {
    Factory stratFactory;
    BarSimRunner sim("NVecho");
    return sim.runWithArgs(argc, argv, &stratFactory);
}
