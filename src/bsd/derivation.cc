#include "bsd/cmd/command.hh"

using namespace bsd;

struct CmdDerivation : BsdMultiCommand
{
    CmdDerivation() : BsdMultiCommand("derivation", RegisterCommand::getCommandsFor({"derivation"}))
    { }

    std::string description() override
    {
        return "Work with derivations, Bsd's notion of a build plan.";
    }

    Category category() override { return catUtility; }
};

static auto rCmdDerivation = registerCommand<CmdDerivation>("derivation");
