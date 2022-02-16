#include <pirate/tr/bar/BarSimRunner.hpp>
using pirate::tr::bar::BarSimRunner;

#include <Vecho-factory.hpp>

int main(int argc, char** argv) {
    Factory stratFactory;
    BarSimRunner sim("Vecho");
    return sim.runWithArgs(argc, argv, &stratFactory);
}
