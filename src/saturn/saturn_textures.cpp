#include "saturn/saturn_textures.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/imgui/saturn_imgui.h"

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"

#include "pc/configfile.h"

extern "C" {
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "sm64.h"
#include "pc/gfx/gfx_pc.h"
}

using namespace std;
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <stdlib.h>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

#include <json/json.h>

bool is_replacing_eyes;

std::vector<string> eye_array;
string current_eye_pre_path;
string current_eye;
string current_eye_dir_path = "dynos/eyes/";
int current_eye_index;
bool model_eyes_enabled;

std::vector<string> mouth_array;
string current_mouth_pre_path;
string current_mouth;
string current_mouth_dir_path;
int current_mouth_index;
bool model_mouth_enabled;

// Eye Folders, Non-Model

void saturn_load_eye_folder(std::string path) {
    eye_array.clear();
    fs::create_directory("res/gfx");

    // reset dir if we last used models or returned to root
    if (path == "../") path = "";
    if (model_eyes_enabled || path == "") {
        model_eyes_enabled = false;
        current_eye_dir_path = "dynos/eyes/";
    }

    // only update current path if folder exists
    if (fs::is_directory(current_eye_dir_path + path)) {
        current_eye_dir_path = current_eye_dir_path + path;
    }

    current_eye_pre_path = "../../" + current_eye_dir_path;

    if (current_eye_dir_path != "dynos/eyes/") {
        eye_array.push_back("../");
    }

    for (const auto & entry : fs::directory_iterator(current_eye_dir_path)) {
        if (fs::is_directory(entry.path())) {
            eye_array.push_back(entry.path().stem().u8string() + "/");
        } else {
            string entryPath = entry.path().filename().u8string();
            if (entryPath.find(".png") != string::npos) // only allow png files
                eye_array.push_back(entryPath);
        }
    }
    
    if (eye_array.size() > 0)
        saturn_set_eye_texture(0);
}

void saturn_eye_selectable(std::string name, int index) {
    if (name.find(".png") != string::npos) {
        // this is an eye
        saturn_set_eye_texture(index);
        std::cout << current_eye << std::endl;
    } else {
        saturn_load_eye_folder(name);
    }
}

std::string current_mouth_folder;
std::string last_folder_name;

/*
    Sets an eye texture with an eye_array index.
*/
void saturn_set_eye_texture(int index) {
    if (eye_array[index].find(".png") == string::npos) {
        // keep trying till we get a non-folder
        saturn_set_eye_texture(index + 1);
        return;
    } else {
        current_eye_index = index;
        current_eye = current_eye_pre_path + eye_array[index];
        current_eye = current_eye.substr(0, current_eye.size() - 4);
    }
}

// NEW SYSTEM, Model

string current_model_exp_tex[6];
bool using_model_eyes;

/*
    Handles texture replacement. Called from gfx_pc.c
*/
const void* saturn_bind_texture(const void* input) {
    const char* inputTexture = static_cast<const char*>(input);
    const char* outputTexture;

    string texName = string(inputTexture);

    if (current_model_data.name != "") {
        for (int i = 0; i < current_model_data.expressions.size(); i++) {

            // Could be either "saturn_eye" or "saturn_eyes", check for both
            string pos_name1 = "saturn_" + current_model_data.expressions[i].name;
            string pos_name2 = pos_name1.substr(0, pos_name1.size() - 1);

            if (texName.find(pos_name1) != string::npos || texName.find(pos_name2) != string::npos) {
                outputTexture = current_model_exp_tex[i].c_str();
                //std::cout << current_model_exp_tex[i] << std::endl;
                const void* output = static_cast<const void*>(outputTexture);
                return output;
            }

        }
    }

    if (texName == "actors/mario/mario_eyes_left_unused.rgba16" || texName.find("saturn_eye") != string::npos) {
        outputTexture = current_eye.c_str();
        const void* output = static_cast<const void*>(outputTexture);
        return output;
    }

    return input;
}

struct ModelData current_model_data;

void saturn_set_model_texture(int expIndex, string path) {
    current_model_exp_tex[expIndex] = "../../" + path;
    current_model_exp_tex[expIndex] = current_model_exp_tex[expIndex].substr(0, current_model_exp_tex[expIndex].size() - 4);
    std::cout << current_model_exp_tex[expIndex] << std::endl;
}

void saturn_load_model_expression_entry(string folder_name, string expression_name) {
    Expression ex_entry;

    // Folder path, could be either something like "eye" OR "eyes"
    string path = "";
    string pos_path1 = "dynos/packs/" + folder_name + "/expressions/" + expression_name + "/";
    string pos_path2 = "dynos/packs/" + folder_name + "/expressions/" + expression_name + "s/";

    // Prefer "eye" over "eyes"
    if (fs::is_directory(pos_path2)) { path = pos_path2; ex_entry.name = (expression_name + "s"); }
    if (fs::is_directory(pos_path1)) { path = pos_path1; ex_entry.name = (expression_name + ""); }
    // If both don't exist, cancel
    if (path == "") { return; }
    if (fs::is_empty(path)) { return; }

    ex_entry.path = path;

    // Load each .png in the path
    for (const auto & entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry.path())) {
            // Ignore, this is a folder
        } else {
            string entryName = entry.path().filename().u8string();
            if (entryName.find(".png") != string::npos) // Only allow .png files
                ex_entry.textures.push_back(entryName);
        }
    }

    if (ex_entry.textures.size() > 0)
        current_model_data.expressions.push_back(ex_entry);
}

/*
    Loads a model.json from a given model (if it exists).
*/
void saturn_load_model_json(std::string folder_name) {
    // Reset current model data
    ModelData blank;
    current_model_data = blank;
    using_model_eyes = false;

    // Load the json file
    std::ifstream file("dynos/packs/" + folder_name + "/model.json");
    if (!file.good()) { return; }

    // Begin reading
    Json::Value root;
    file >> root;

    current_model_data.name = root["name"].asString();
    current_model_data.author = root["author"].asString();
    current_model_data.version = root["version"].asString();

    if (root.isMember("textures")) {
        // Create res/gfx if it doesn't already exist
        fs::create_directory("res/gfx");

        // Texture entries : eyes, mouths
        for(int i = 0; i < root["textures"].size(); i++) {
            // Capped at 6
            if (i > 6) break;

            const char* index = std::to_string(i).c_str();
            string expression_name = root["textures"][index].asString();
            saturn_load_model_expression_entry(folder_name, expression_name);

            // Choose first texture as default
            current_model_exp_tex[i] = "../../" + current_model_data.expressions[i].path + current_model_data.expressions[i].textures[0];
            current_model_exp_tex[i] = current_model_exp_tex[i].substr(0, current_model_exp_tex[i].size() - 4);

            // Toggle model eyes
            if (expression_name.find("eye") != string::npos) using_model_eyes = true;
        }
    }
}

void saturn_load_model_data(std::string folder_name) {
    // Reset current model data
    ModelData blank;
    current_model_data = blank;
    using_model_eyes = false;

    // Load the json file
    std::ifstream file("dynos/packs/" + folder_name + "/model.json");
    if (file.good()) {
        // Begin reading
        Json::Value root;
        file >> root;

        current_model_data.name = root["name"].asString();
        current_model_data.author = root["author"].asString();
        current_model_data.version = root["version"].asString();

        // CC support is enabled by default, SPARK is disabled
        // This is just in case it wasn't defined in the model.json
        current_model_data.cc_support = true;
        current_model_data.spark_support = false;

        if (root.isMember("cc_support")) {
            current_model_data.cc_support = root["cc_support"].asBool();
            cc_model_support = current_model_data.cc_support;
        }
        
        if (root.isMember("spark_support")) {
            current_model_data.spark_support = root["spark_support"].asBool();
            cc_spark_support = current_model_data.spark_support;

            // If SPARK is enabled, enable CC support too (it needs it to work)
            if (current_model_data.spark_support == true) {
                current_model_data.cc_support = true;
                cc_model_support = true;
            }
        }
    }

    string path = "dynos/packs/" + folder_name + "/expressions/";
    if (!fs::is_directory(path)) return;

    int i = 0;
    for (const auto & entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry.path())) {
            string expression_name = entry.path().filename().u8string();
            saturn_load_model_expression_entry(folder_name, expression_name);
            
            // Choose first texture as default
            current_model_exp_tex[i] = "../../" + current_model_data.expressions[i].path + current_model_data.expressions[i].textures[0];
            current_model_exp_tex[i] = current_model_exp_tex[i].substr(0, current_model_exp_tex[i].size() - 4);

            // Toggle model eyes
            if (expression_name.find("eye") != string::npos) using_model_eyes = true;

            i++;
        } else {
            // Ignore, these are files
        }
    }
}