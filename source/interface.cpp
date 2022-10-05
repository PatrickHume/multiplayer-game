#include"../headers/interface.h"

// Constructor that generates a Elements Buffer Object and links it to indices
Interface::Interface(int width, int height)
{
    Interface::width = width;
    Interface::height = height;

    mapStringValues["summon"]         = CmdWord::SUMMON;
    mapStringValues["show"]           = CmdWord::SHOW;
    mapStringValues["add"]            = CmdWord::ADD;
    mapStringValues["remove"]         = CmdWord::REMOVE;
    mapStringValues["end"]            = CmdWord::END;

    std::cout << "mapStringValues contains "
    << mapStringValues.size()
    << " entries." << std::endl;
}

void Interface::toggleCmdLine()
{
    cmdLineOpen = !cmdLineOpen;
}

void Interface::typeChar(char c){
    cmdInput.push_back(c);
}
void Interface::backspace(){
    cmdInput.pop_back();
}

bool Interface::checkCmd(){
    if(cmdInput.size() == 0){
        return false;
    }
    if (!cmdLineOpen) {
        return false;
    }
    if(cmdInput.size() > 32){
        return false;
    }
    return true;
}

void Interface::Draw(TextRenderer &textRenderer, Shader &textShader, double time){
    const float x = 25.0f;
    const float y = 25.0f;
    //height of text
    const float h = 20.0f;
    //size of text
    const float s = 0.3f;

    if(cmdLineOpen){
        textRenderer.renderText(textShader, cmdInput + (fmod(time,1.0) > 0.5 ? "_":""), x, y, s, glm::vec3(1.0f, 1.0f, 1.0f));
        for (int i = 0; i < cmdHistory.size(); i++){
            textRenderer.renderText(textShader, cmdHistory[i], x, y+((cmdHistory.size()-i)*h), s, glm::vec3(0.7f, 0.7f, 0.7f));
        }
    }
}

std::vector<CmdWord> Interface::getCmd(){
    std::vector<std::string> cmdStrings = split(cmdInput,' ');
    std::vector<CmdWord>     cmdWords;
    cmdHistory.push_back(cmdInput);
    cmdInput.clear();

    if(cmdStrings.size() > 5){
        cmdHistory.push_back("ERROR: Command too long.");
        return std::vector<CmdWord>(1,CmdWord::ERROR);
    }
    for (int i = 0; i < cmdStrings.size(); i++){
        cmdWords.push_back(mapStringValues[cmdStrings[i]]);
    }
    return cmdWords;
}
