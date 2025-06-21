#include "bsd/cmd/command.hh"

using namespace bsd;

struct CmdStore : BsdMultiCommand
{
    CmdStore() : BsdMultiCommand("store", RegisterCommand::getCommandsFor({"store"}))
    {
        aliases = {
            {"ping", { AliasStatus::Deprecated, {"info"}}},
        };
    }

    std::string description() override
    {
        return "manipulate a Bsd store";
    }

    Category category() override { return catUtility; }
};

static auto rCmdStore = registerCommand<CmdStore>("store");
