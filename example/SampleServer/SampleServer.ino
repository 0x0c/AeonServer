#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>
#include <string>
#include <vector>
#include <iterator>
#include "U8glib.h"
#include <AeonFoundation.h>
#include <AeonUI.h>
// #include <M2tk.h>
// #include "utility/m2ghu8g.h"

uint8_t uiKeyUpPin = 7;
uint8_t uiKeyDownPin = 6;
uint8_t uiKeyLeftPin = 5;
uint8_t uiKeyRightPin = 4;
uint8_t uiKeySelectPin = 3;
// 2
// 1 -> TX
// 0 -> RX
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);

int global_id_counter = 0;
bool need_to_draw = true;

typedef enum {
	Waiting,
	DrawModeLine,
	DrawModeRect,
	DrawModeLabel,
	DrawModeEdit,
	DeleteMode
} State;

State s;

std::vector<Aeon::Object *> array;

void read_2_point_data()
{
	int data_length = Serial.available();
	Aeon::Point p[2];
	for (int i = 0; i < data_length; i++) {
		int data = Serial.read();
		int index = i > 1 ? 1 : 0;
		if (i % 2) {
			p[index].y = data;
		}
		else {
			p[index].x = data;
		}
	}
	if (s == DrawModeLine) {
		Aeon::Line *line = new Aeon::Line();
		read_2_point_data();
		line->context = &u8g;
		line->id = array.size();
		array.push_back(line);
	}
	else if (s == DrawModeRect) {
		Aeon::Rect *rect = new Aeon::Rect(p[0], p[1]);
		rect->context = &u8g;
		rect->id = array.size();
		array.push_back(rect);
	}
	
	s = Waiting;
}

void set_2_point_data(Aeon::Object *obj)
{
	int data_length = Serial.available();
	Aeon::Point p[2];
	for (int i = 0; i < data_length; i++) {
		int data = Serial.read();
		int index = i > 1 ? 1 : 0;
		if (i % 2) {
			p[index].y = data;
		}
		else {
			p[index].x = data;
		}
	}
	if (obj->type == Aeon::ObjectTypeLine) {
		Aeon::Line *line = static_cast<Aeon::Line *>(obj);
		line->start = p[0];
		line->end = p[1];
	}
	else if (obj->type == Aeon::ObjectTypeRect) {
		Aeon::Rect *rect = static_cast<Aeon::Rect *>(obj);
		rect->start = p[0];
		rect->end = p[1];
		array.push_back(rect);
	}
	
	s = Waiting;
}

void set_1_point_data(Aeon::Object *obj)
{
	if (obj->type == Aeon::ObjectTypeLabel) {
		int data_length = Serial.available();
		Aeon::Point p;
		std::string text("");
		for (int i = 0; i < 2; i++) {
			int data = Serial.read();
			if (i % 2) {
				p.y = data + 13;
			}
			else {
				p.x = data;
			}
		}
		int text_length = Serial.available();
		for (int i = 0; i < text_length; i++) {
			char c = Serial.read();
			text.append(1, c);
		}
		text.append(1, '\0');
		Aeon::Label *label = static_cast<Aeon::Label *>(obj);
		label->position = p;
		label->text = text;
	}

	s = Waiting;
}

void read_data(void)
{
	if (Serial.available() > 0) {
		if (s == Waiting) {
			char c = Serial.read();
			if (c == 'l') {
				s = DrawModeLine;
			}
			if (c == 'r') {
				s = DrawModeRect;
			}
			if (c == 't') {
			    s = DrawModeLabel;
			}
			if (c == 'd') {
				s = DeleteMode;
			}
			if (c == 'x') {
				s = DrawModeEdit;
			}
		}
		else {
			if (s == DrawModeLine) {
				Aeon::Line *line = new Aeon::Line();
				line->context = &u8g;
				line->id = global_id_counter++;
				set_2_point_data(line);
				array.push_back(line);
			}
			else if (s == DrawModeRect) {
				Aeon::Rect *rect = new Aeon::Rect();
				rect->context = &u8g;
				rect->id = global_id_counter++;
				set_2_point_data(rect);
				array.push_back(rect);
			}
			else if (s == DrawModeLabel) {
			    // read_label();
			}
			else if (s == DeleteMode) {
				int index = Serial.read();
				array.erase(array.begin() + index);
			}
			else if (s == DrawModeEdit) {
			    int index = Serial.read();
			    set_2_point_data(array.at(index));
			}

			need_to_draw = true;
		}
	}
}

// // Edit the following long int number
// uint32_t number = 1234;

// // Definition of the m2tklib menu
// void fn_ok(m2_el_fnarg_p fnarg) {
//   /* do something with the number */
// }
// M2_LABEL(el_label, NULL, "Num: ");
// M2_U32NUM(el_num, "a1c4", &number);
// M2_BUTTON(el_ok, "", " ok ", fn_ok);
// M2_LIST(list) = { &el_label, &el_num, &el_ok };
// M2_HLIST(top_el_hlist, NULL, list);

// M2tk m2(&top_el_hlist, m2_es_arduino, m2_eh_2bs, m2_gh_u8g_bfs);

AeonUI::Page page;

void draw()
{
	page.draw();
	// m2.draw();
	// for (int i = 0; i < array.size(); i++) {
	// 	Aeon::Object *obj = array.at(i);
	// 	obj->draw();
	// }
}

void setup(void)
{
	// flip screen, if required
	// u8g.setRot90();
	// u8g.setRot180();

	Serial.begin(9600);

	// set REST HIGH
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	// /* connect u8glib with m2tklib */
	// m2_SetU8g(u8g.getU8g(), m2_u8g_box_icon);
	// /* assign u8g font to index 0 */
	// m2.setFont(0, u8g_font_7x13);
	// // Setup keys
	// m2.setPin(M2_KEY_SELECT, uiKeySelectPin);
	// m2.setPin(M2_KEY_NEXT, uiKeyDownPin);
}

void loop(void)
{
	// read_data();
	// m2.checkKey();
	if (1 && page.needToRefresh) {
		u8g.firstPage();
		do {
			draw();
		} while(u8g.nextPage());
	}
	
	delay(200);
}
