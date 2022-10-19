#ifndef INTERFACE_CLASS_H
#define INTERFACE_CLASS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <memory>
#include "stringSwitch.h"
#include "textRenderer.h"
#include "camera.h"

enum class MessageType {REGULAR,
                        DATA,
                        ERROR};

struct Message {
    MessageType messageType;
    std::string messageText;
};

struct CommandRuleset{
    std::string name;
    std::string labelRule;  // this is the rule in its label form i.e.  summon <objectType> at <position>
    std::string typeRule;   // this is the rule in its type form i.e.   summon (object) at (float) (float) (float)
    std::string regexRule;  // this is the rule in its regex form i.e.  summon [lada|cube] at (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)?
    std::regex  regex;
    int         numOfArgs;
};

struct Command{
    std::string name;
    std::vector<std::string> parameters;
};

class Interface
{
public:
	// Constructor that generates a Elements Buffer Object and links it to indices
	Interface();
    void toggleCmdLine();
    // time is used in draw to add a flashing cursor effect
    void Draw(TextRenderer &textRenderer, std::shared_ptr<Shader>&textShader, double time = 0.0);

    Command getCmd();
    bool checkCmd();

    void typeChar(char c);
    void backspace();
    void submitCmd();

    void write(Message message);

    bool cmdReady       = false;
    bool cmdLineOpen    = false;

    int width;
    int height;

    Camera* camera;

private:
    const std::regex findNeumonicRegex    = std::regex("^[A-z]+(?=:\\s)");
    const std::regex findLabelRegex       = std::regex("^<[A-z0-9\\.\\s]+>(?= -> )");
    const std::regex findTypeRegex        = std::regex("^\\([A-z0-9\\.\\s]+\\)(?= -> )");
    const std::regex findSpacesRegex      = std::regex("\\s\\s+");
    const std::regex findString           = std::regex("string");
    const std::regex findFloat            = std::regex("float");

    std::map<MessageType, glm::vec3> mapMsgTypeToCol = {
        {MessageType::REGULAR,  glm::vec3(0.85f, 0.85f, 0.85f)},
        {MessageType::DATA,     glm::vec3(0.19f, 0.94f, 1.0f)},
        {MessageType::ERROR,    glm::vec3(1.0f, 0.25f, 0.19f)}
    };
    std::map<MessageType, std::string> mapMsgTypeToPrefix = {
        {MessageType::REGULAR,  ""},
        {MessageType::DATA,     "    "},
        {MessageType::ERROR,    "Error: "}
    };

    std::vector<std::pair<std::regex, std::string>> typeRules;
    std::vector<std::pair<std::regex, std::string>> regexRules;
    std::vector<CommandRuleset> commandRulesets;
    std::vector<Message> cmdHistory;
    std::string cmdInput;

    Message commandNotFoundMessage{
        MessageType::ERROR,
        "command not found",
    };
};

#endif
