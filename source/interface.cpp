#include"../headers/interface.h"

// Constructor that generates a Elements Buffer Object and links it to indices
Interface::Interface()
{
    std::regex findNeumonicRegex("^[A-z]+(?=:\\s)");
    std::regex findLabelRegex("^<[A-z0-9\\.\\s]+>(?= -> )");
    std::regex findTypeRegex("^\\([A-z0-9\\.\\s]+\\)(?= -> )");

    //std::regex findInputRegex("\\([A-z0-9\\.\\s]+\\)");
    std::regex findSpacesRegex("\\s\\s+");

    std::regex findString("string");
    std::regex findFloat("float");

    //map message type to colors
    mapMsgTypeToCol[MessageType::REGULAR]   = glm::vec3(0.7f, 0.7f, 0.7f);
    mapMsgTypeToCol[MessageType::ERROR]     = glm::vec3(1.0f, 0.44f, 0.39f);

    //map oject names to enums
    mapObjects["lada"]         = ObjectID::LADA;
    mapObjects["cube"]         = ObjectID::CUBE;

    //map neumonics to enums
    mapNeumonics["so"]         = CommandID::SUMMON_OBJECT;
    mapNeumonics["soap"]       = CommandID::SUMMON_OBJECT_AT_POS;
    mapNeumonics["lo"]         = CommandID::LIST_OBJECTS;
    mapNeumonics["sc"]         = CommandID::SHOW_COLLIDERS;
    mapNeumonics["hc"]         = CommandID::HIDE_COLLIDERS;

    std::cout << "mapStringValues contains "
    << mapNeumonics.size()
    << " entries." << std::endl;

    // open commands.txt and separate the typeRules and the commandRulesets
    std::string line;
    std::ifstream cmdFile;
    cmdFile.open("resources/commands.txt");
    if (!cmdFile.is_open())
        throw std::runtime_error("Interface commandRulesets file could not be opened.");

    std::string labelRule;  // this is the rule in its label form i.e.  summon <objectType> at <position>
    std::string typeRule;   // this is the rule in its type form i.e.   summon (object) at (float) (float) (float)
    std::string regexRule;  // this is the rule in its regex form i.e.  summon [lada|cube] at (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)?
       
    // cycle through command.txt lines and classify lines
    while (getline (cmdFile,line))
    {
        // this line in commands.txt is empty
        if(line.length() == 0){
            continue;
        }
        // this line in commands.txt is a comment
        if(line.at(0) == '/')
        {
            continue;
        }
        line = regex_replace(line, findSpacesRegex, " ");
        // this is a rule linking labels to types
        // i.e. <...> -> (...)
        if(line.at(0) == '<'){
            std::smatch findMatch;
            regex_search(line, findMatch, findLabelRegex);
            // throw error if no match found
            if(findMatch.str(0).length() == 0)
                throw std::runtime_error("Interface found illegal command rule in label");
            std::regex findResult(findMatch.str(0).c_str());
            // + 4 is to take into account the " -> " used in rule setting
            std::string replaceWith = line.substr(findMatch.position() + 4 + findMatch.length());
            typeRules.push_back(std::pair<std::regex, std::string>(findResult,replaceWith));

        // this is a rule linking types to regex, which is used to verify commandRulesets
        // i.e. (...) -> some regex expression
        }else if(line.at(0) == '('){
            std::smatch findMatch;
            regex_search(line, findMatch, findTypeRegex);
            // throw error if no match found
            if(findMatch.str(0).length() == 0)
                throw std::runtime_error("Interface found illegal command rule in type");
            // this monstorous line takes a string such as "(float)"
            // and turns it into "\(float\)", so it will match "(float)" when used as a regular expression
            std::regex findResult(regex_replace(regex_replace(findMatch.str(0),std::regex("\\("),"\\\("),std::regex("\\)"),"\\\)").c_str());
            // + 4 is to take into account the " -> " used in rule setting
            std::string replaceWith = line.substr(findMatch.position() + 4 + findMatch.length());
            regexRules.push_back(std::pair<std::regex, std::string>(findResult,replaceWith));

        // this line in commands.txt is a command
        }else{
            std::smatch findMatch;
            regex_search(line, findMatch, findNeumonicRegex);
            // throw error if no neumonic found
            if(findMatch.str(0).length() == 0)
                throw std::runtime_error("Interface found illegal command rule in type");

            std::string neumonic = findMatch.str(0);

            if (mapNeumonics.find(neumonic) == mapNeumonics.end()) {
                throw std::runtime_error("Interface couldnt find neumonic \""+neumonic+"\" in mapNeumonics.");
            }

            CommandID id = mapNeumonics[neumonic];
            // now remove the neumonic and its remaining ": " part from the line
            std::string labelRule = line.substr(findMatch.position() + 2 + findMatch.length());

            // begin building the command group
            // we will use the first label rule to populate the type and regex ruless
            CommandRuleset commandRuleset{
                id,
                labelRule,      // (labelRule) this is the rule in its label form i.e.  summon <objectType> at <position>
                std::string(),  // (typeRule) this is the rule in its type form i.e.    summon (object) at (float) (float) (float)
                std::string(),  // (regexRule) this is the rule in its regex form i.e.  summon [lada|cube] at (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)? (\.\d+)|\d+(\.\d+)?
                std::regex(),   // (regex) this is the actual regex used to match
                0               // (numOfArgs)
            };
            commandRulesets.push_back(commandRuleset);

        }
    }
    cmdFile.close();
    //break down labels <...> into types (...)
    for(int i = 0; i < commandRulesets.size(); i++){
        std::string typeCommand = commandRulesets[i].labelRule;
        // apply label->type replacements
        for(int j = 0; j < typeRules.size(); j++){
            typeCommand = regex_replace(typeCommand, typeRules[j].first, typeRules[j].second);
        }
        commandRulesets[i].typeRule = typeCommand;

        // record number of inputs to the command (number of types)
        // this is done by finding the number of bracket pairs
        int numOfArgs = 0;
        bool openBracket = false;
        for(int j = 0; j < typeCommand.length(); j++){
            if(typeCommand.at(j) == '(' && !openBracket){
                openBracket = true;
                numOfArgs+=1;
                continue;
            }
            if(typeCommand.at(j) == ')' && openBracket){
                openBracket = false;
                continue;
            }           
        }
        if(openBracket == true){
            throw std::runtime_error("Brackets mismatch in Interface command parsing");
        }
        commandRulesets[i].numOfArgs = numOfArgs;
    }

    // break down types (...) into regex
    for(int i = 0; i < commandRulesets.size(); i++){
        std::string regexCommand = commandRulesets[i].typeRule;
        // apply type->regex replacements
        for(int j = 0; j < regexRules.size(); j++){
            regexCommand = regex_replace(regexCommand, regexRules[j].first, regexRules[j].second);
        }
        commandRulesets[i].regexRule = "^"+regexCommand+"$";
        commandRulesets[i].regex = std::regex(commandRulesets[i].regexRule);
    }

    /*
    // display each command group
    std::cout << "--------------" << std::endl;
    for(int i = 0; i < commandRulesets.size(); i++){
        std::cout << "id:        " << commandRulesets[i].id << std::endl;
        std::cout << "labelRule: " << commandRulesets[i].labelRule << std::endl;
        std::cout << "typeRule:  " << commandRulesets[i].typeRule << std::endl;
        std::cout << "regexRule: " << commandRulesets[i].regexRule << std::endl;
        std::cout << "" << std::endl;
    }
    std::cout << "--------------" << std::endl;
    */

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
            textRenderer.renderText(textShader, cmdHistory[i].messageText, x, y+((cmdHistory.size()-i)*h), s, mapMsgTypeToCol[cmdHistory[i].messageType]);
        }
    }
}

Command Interface::getCmd(){
    //clear the text box
    Message message {
        MessageType::REGULAR,
        cmdInput
    };
    cmdHistory.push_back(message);

    //prepare command to send to main
    Command command;
    CommandID id = CommandID::ERROR;
    std::vector<std::string> parameters;

    std::smatch findMatch;
    // check if any command matches input
    for(int i = 0; i < commandRulesets.size(); i++){
        regex_search(cmdInput, findMatch, commandRulesets[i].regex);
        if(findMatch.str(0).length() == 0)
            continue;
        CommandRuleset commandRuleset = commandRulesets[i];

        id = commandRuleset.id;
        if(findMatch.size()-1 != commandRuleset.numOfArgs)
            std::cout << findMatch.size()-1 << " != " << commandRuleset.numOfArgs << std::endl;
            //throw std::runtime_error("Num of args in interface command doesn't match num in commandRuleset.");

        for(int i = 1; i < findMatch.size(); i++){
            parameters.push_back(findMatch.str(i));
        }
        break;
    }

    command.id = id;
    command.parameters = parameters;

    if (id == CommandID::ERROR){
        cmdHistory.push_back(commandNotFoundMessage);
    }

    // clear the command input space
    cmdInput.clear();
    // send the command to main
    return command;
}

ObjectID Interface::mapObject(std::string& name){
    if (mapObjects.find(name) == mapObjects.end()) {
        throw std::runtime_error("Interface couldnt find \""+name+"\" in mapObjects.");
    }
    return mapObjects[name];
}
