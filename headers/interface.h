#ifndef INTERFACE_CLASS_H
#define INTERFACE_CLASS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include "stringSwitch.h"
#include "textRenderer.h"
#include "camera.h"

// Value-Defintions of the different String values
enum class CommandID {  SUMMON_OBJECT,
                        SUMMON_OBJECT_AT_POS,
                        LIST_OBJECTS,
                        SHOW_COLLIDERS,
                        HIDE_COLLIDERS,
                        ERROR };

enum class ObjectID {   LADA,
                        CUBE};

enum class MessageType {REGULAR,
                        ERROR};

struct Message {
    MessageType messageType;
    std::string messageText;
};

struct CommandRuleset{
    CommandID id;
    std::string labelRule;  // this is the rule in its label form i.e.  summon <objectType> at <position>
    std::string typeRule;   // this is the rule in its type form i.e.   summon (object) at (float) (float) (float)
    std::string regexRule;  // this is the rule in its regex form i.e.  summon [lada|cube] at (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)?
    std::regex  regex;
    int         numOfArgs;
};

struct Command{
    CommandID id;
    std::vector<std::string> parameters;
};

class Interface
{
public:
	// Constructor that generates a Elements Buffer Object and links it to indices
	Interface();
    void toggleCmdLine();
    // time is used in draw to add a flashing cursor effect
    void Draw(TextRenderer &textRenderer, Shader &textShader, double time = 0.0);

    Command getCmd();
    bool checkCmd();

    void typeChar(char c);
    void backspace();
    void submitCmd();

    bool cmdReady       = false;
    bool cmdLineOpen    = false;

    int width;
    int height;

    Camera* camera;

    ObjectID mapObject(std::string& name);
private:
    std::map<std::string, CommandID>    mapNeumonics;
    std::map<std::string, ObjectID>     mapObjects;
    std::map<MessageType, glm::vec3>    mapMsgTypeToCol;
    std::vector<std::pair<std::regex, std::string>> typeRules;
    std::vector<std::pair<std::regex, std::string>> regexRules;
    std::vector<CommandRuleset> commandRulesets;
    std::vector<Message> cmdHistory;
    std::string cmdInput;

    Message commandNotFoundMessage{
        MessageType::ERROR,
        "Error: command not found",
    };
};

#endif
