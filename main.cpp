/**************************************************************************
 * Robert Borghese
 *
 * main.cpp - SRD_ElementalSkillColors
 **************************************************************************/

/*
 * Kinda lazy, so not going to document this too hard, but here ya go...
 */

#include <cmath>
#include <nan.h>
#include <vector>
#include <iostream>
#include <string>
#include <vector>

/*
 * Double namespace situation to accomidate for future plugins under "SumRndmDde".
 * Parameter values remain in global namespace for convenience and because that's the style of most plugins.
 */

namespace SumRndmDde {
	namespace ElementalSkillColors {
		std::vector<int> colors;
		bool affectSkills;
		bool affectItems;
	}
}

/*
 * Just makes things easier for us in the future.
 */

#define SRD_ESC SumRndmDde::ElementalSkillColors


/*
 * The overwritten C++ version of Window_Base.prototype.drawItemName.
 * Note that this has a lot of code in order to replicate existing JavaScript code.
 * Normally, this wouldn't be necessary, but this plugin requires overwritting.
 * Realistically, one would simply need to call the original function and append new C++ code.
 */
void drawItemName(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	auto _this = info.This().As<v8::Object>();

	/*
	 * Check the parameters...
	 */

	if(info.Length() >= 3 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber()) {

		/*
		 * Store necessary fields into variables.
		 */

		v8::Isolate* isolate = info.GetIsolate();
		auto global = isolate->GetCurrentContext()->Global();
		auto window = global->Get(v8::String::NewFromUtf8(isolate, "window")).As<v8::Object>();

		auto DataManager = window->Get(v8::String::NewFromUtf8(isolate, "DataManager")).As<v8::Object>();
		auto isSkill = DataManager->Get(v8::String::NewFromUtf8(isolate, "isSkill")).As<v8::Function>();
		auto isItem = DataManager->Get(v8::String::NewFromUtf8(isolate, "isItem")).As<v8::Function>();

		auto Window_Base = window->Get(v8::String::NewFromUtf8(isolate, "Window_Base")).As<v8::Object>();
		auto _iconWidth = Window_Base->Get(v8::String::NewFromUtf8(isolate, "_iconWidth"))->IntegerValue() + 4;

		auto item = info[0]->ToObject();
		double x = static_cast<double>(info[1]->IntegerValue());
		double y = static_cast<double>(info[2]->IntegerValue());
		double w = (info.Length() == 3 || (info.Length() > 3 && !info[3]->IsNumber())) ? 312.0 : static_cast<double>(info[3]->IntegerValue());

		v8::Local<v8::Value> isSkillItemArgs[1] = { item };

		auto isItemSkill = isSkill->Call(DataManager, 1, isSkillItemArgs)->BooleanValue();
		auto isItemItem = isItem->Call(DataManager, 1, isSkillItemArgs)->BooleanValue();

		bool shouldColor = (isItemSkill && SRD_ESC::affectSkills) || (isItemItem && SRD_ESC::affectItems);

		auto drawIcon = _this->Get(v8::String::NewFromUtf8(isolate, "drawIcon")).As<v8::Function>();
		auto drawText = _this->Get(v8::String::NewFromUtf8(isolate, "drawText")).As<v8::Function>();
		auto changeTextColor = _this->Get(v8::String::NewFromUtf8(isolate, "changeTextColor")).As<v8::Function>();
		auto textColor = _this->Get(v8::String::NewFromUtf8(isolate, "textColor")).As<v8::Function>();

		auto damage = item->Get(v8::String::NewFromUtf8(isolate, "damage")).As<v8::Object>();
		auto elementId = damage->Get(v8::String::NewFromUtf8(isolate, "elementId"))->IntegerValue();

		/*
		 * Replicate JavaScript code.
		 */

		if(shouldColor && elementId > 0 && elementId <= 30) {

			v8::Local<v8::Value> textColorArgs[1] = {
				v8::Number::New(isolate, SRD_ESC::colors.at(static_cast<unsigned int>(elementId)))
			};

			v8::Local<v8::Value> changeTextColorArgs[1] = {
				textColor->Call(_this, 1, textColorArgs).As<v8::String>()
			};

			changeTextColor->Call(_this, 1, changeTextColorArgs);

		} else {

			auto resetTextColor = _this->Get(v8::String::NewFromUtf8(isolate, "resetTextColor")).As<v8::Function>();
			resetTextColor->Call(_this, 0, nullptr);

		}

		v8::Local<v8::Value> drawIconArgs[3] = {
			item->Get(v8::String::NewFromUtf8(isolate, "iconIndex")).As<v8::Number>(),
			v8::Number::New(isolate, x + 2),
			v8::Number::New(isolate, y + 2)
		};

		v8::Local<v8::Value> drawTextArgs[4] = {
			item->Get(v8::String::NewFromUtf8(isolate, "name")).As<v8::Number>(),
			v8::Number::New(isolate, x + _iconWidth),
			v8::Number::New(isolate, y),
			v8::Number::New(isolate, w - _iconWidth)
		};

		drawIcon->Call(_this, 3, drawIconArgs);
		drawText->Call(_this, 4, drawTextArgs);
	} else {

		/*
		 * If the wrong parameters are provided, original function is called.
		 */

		auto originalFunction = v8::Local<v8::Function>::Cast(info.Data());
		std::vector<v8::Local<v8::Value>> arguments(info.Length());
		for(int i = 0; i < info.Length(); i++) {
			arguments.at(i) = info[i];
		}
		originalFunction->Call(_this, info.Length(), &arguments[0]);
	}
}

void ApplyPlugin(const Nan::FunctionCallbackInfo<v8::Value>& info) {

	/*
	 * Grab necessary values and store into variables.
	 */

	v8::Isolate* isolate = info.GetIsolate();
	auto global = isolate->GetCurrentContext()->Global();
	auto window = global->Get(v8::String::NewFromUtf8(isolate, "window")).As<v8::Object>();

	SRD_ESC::colors = std::vector<int>(31);

	v8::Local<v8::Object> SRD_ElementalSkillColors = v8::Object::New(isolate);

	auto PluginManager = window->Get(v8::String::NewFromUtf8(isolate, "PluginManager")).As<v8::Object>();
	auto parameters = PluginManager->Get(v8::String::NewFromUtf8(isolate, "parameters")).As<v8::Function>();

	v8::Local<v8::Value> parametersArgs[1] = {
		v8::String::NewFromUtf8(isolate, "SRD_ElementalSkillColors")
	};
	auto resultingParameters = parameters->Call(PluginManager, 1, parametersArgs).As<v8::Object>();

	/*
	 * Transfer parameter data into SumRndmDde::ElementalSkillColors namespace
	 */

	for(int i = 1; i <= 30; i++) {
		std::string paramName = std::string("Element ID ") + std::to_string(i);
		SRD_ESC::colors[i] = static_cast<int>(resultingParameters->Get(v8::String::NewFromUtf8(isolate, paramName.c_str()))->IntegerValue());
	}

	SRD_ESC::affectSkills = resultingParameters->Get(v8::String::NewFromUtf8(isolate, "Affect Skills?"))->Equals(v8::String::NewFromUtf8(isolate, "true"));
	SRD_ESC::affectItems = resultingParameters->Get(v8::String::NewFromUtf8(isolate, "Affect Items?"))->Equals(v8::String::NewFromUtf8(isolate, "true"));

	/*
	 * Apply changes to Window_Base.prototype.drawItemName function.
	 */

	auto Window_Base = window->Get(v8::String::NewFromUtf8(isolate, "Window_Base")).As<v8::Object>();
	auto Window_Base_prototype = Window_Base->Get(v8::String::NewFromUtf8(isolate, "prototype")).As<v8::Object>();

	auto Window_Base_drawItemName = Window_Base_prototype->Get(v8::String::NewFromUtf8(isolate, "drawItemName")).As<v8::Function>();
	auto Window_Base_drawItemName_new = Nan::New<v8::FunctionTemplate>(drawItemName, Window_Base_drawItemName);
	
	Window_Base_prototype->Set(v8::String::NewFromUtf8(isolate, "drawItemName"), Window_Base_drawItemName_new->GetFunction());
	Window_Base->Set(v8::String::NewFromUtf8(isolate, "prototype"), Window_Base_prototype);
	window->Set(v8::String::NewFromUtf8(isolate, "Window_Base"), Window_Base);
	window->Set(v8::String::NewFromUtf8(isolate, "SRD_ElementalSkillColors"), SRD_ElementalSkillColors);
}

#undef SRD_ESC

/*
 * Do other stuff needed for node-gyp ¯\_(ツ)_/¯
 */

void Init(v8::Local<v8::Object> exports) {  
	exports->Set(Nan::New("applyPlugin").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(ApplyPlugin)->GetFunction());
}

NODE_MODULE(SRD_ElementalSkillColors, Init)