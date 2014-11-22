#pragma once

#include <StandardCplusplus.h>
#include <string>
#include <vector>
#include <functional>
#include "U8glib.h"
#include <AeonUI.h>
#include <Arduino.h>

namespace AeonServer
{
	using namespace AeonUI;
	typedef enum {
		Waiting,
		Processing
	} State;
	class Server
	{
	private:
		int globalIdentifierCounter;
		Page *page;
		Point pointBuffer[2];
		int dataBuffer[2];
		std::string stringBuffer;
		char command;
		int step;
	public:
		Server() {
			this->step = 0;
			this->command = '\0';
			this->globalIdentifierCounter = 1;
		}
		~Server() {
		};
		void registerPage(Page *page) {
			this->page = page;
		}
		void receive() {
			if (Serial.available() > 0) {
				if (this->command == '\0') {
					this->command = Serial.read();
				}
				else {
					this->processCommand(this->command);
				}
			}
		}
		void readPoints(int count) {
			int index = 0;
			for (int i = 0; i < count * 2; i++) {
				int coordinate = Serial.read();
				if (i % 2 == 0) {
					pointBuffer[index].x = coordinate;
				}
				else {
					pointBuffer[index].y = coordinate;
					index++;
				}
			}
		}
		void readData(int count) {
			for (int i = 0; i < count; i++) {
				int data = Serial.read();
				this->dataBuffer[i] = data;
			}
		}
		std::string readString() {
			return "";
		}
		void processCommand(char comm) {
			switch (comm) {
				case 'b': {
					if (Serial.available() > 3) {
						this->readPoints(2);
						Button *button = new Button(this->pointBuffer[0], this->pointBuffer[1]);
						button->identifier = this->globalIdentifierCounter++;
						button->roundRect = true;
						button->text = "srvr";
						this->page->add(button);
						this->returnValue(button->identifier);
						this->command = '\0';
					}
				}
					break;
				case 's': {
					if (Serial.available() > 3) {
						this->readPoints(2);
						Switch *sw = new Switch(this->pointBuffer[0], this->pointBuffer[1]);
						this->page->add(sw);
						sw->identifier = this->globalIdentifierCounter++;
						Serial.print(sw->identifier);
						this->command = '\0';
					}
				}
					break;
				case 'l': {
					if (Serial.available() > 1) {
						if (this->step == 0) {
							this->readPoints(1);
							this->step = 1;
						}
						else if (this->step == 1) {
							if (this->readString() == false) {
								this->step = 2;
							}
						}
						else {
							Label *label = new Label(this->pointBuffer[0], this->stringBuffer);
							this->page->add(label);
							label->identifier = this->globalIdentifierCounter++;
							Serial.print(label->identifier);

							this->stringBuffer = "";
							this->command = '\0';
						}
					}
				}
					break;
				case 't': {
					List *list = new List();
					this->page->add(list);
					list->identifier = this->globalIdentifierCounter++;
					Serial.print(list->identifier);
					this->command = '\0';
				}
					break;
				default:
					this->command = '\0';
					break;
			}
		}
		void returnValue(int value) {
			Serial.print(value);
		}
	};
};