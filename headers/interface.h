#ifndef INTERFACE_CLASS_H
#define INTERFACE_CLASS_H

#include <string>
#include <vector>
#include "stringSwitch.h"
#include "textRenderer.h"
#include "camera.h"

// Value-Defintions of the different String values
enum CmdWord {  NOT_DEFINED,
                    SUMMON,
                    SHOW,
                    REMOVE,
                    ADD,
                    ERROR,
                    END };

class Interface
{
public:
	// Constructor that generates a Elements Buffer Object and links it to indices
	Interface(int width, int height);
    void toggleCmdLine();
    // time is used in draw to add a flashing cursor effect
    void Draw(TextRenderer &textRenderer, Shader &textShader, double time = 0.0);

    std::vector<CmdWord> getCmd();
    bool checkCmd();

    void typeChar(char c);
    void backspace();
    void submitCmd();

    bool cmdReady       = false;
    bool cmdLineOpen    = false;

    int width;
    int height;

    Camera* camera;
private:
    std::map<std::string, CmdWord> mapStringValues;
    std::vector<std::string> cmdHistory;
    std::string cmdInput;
};

#endif
