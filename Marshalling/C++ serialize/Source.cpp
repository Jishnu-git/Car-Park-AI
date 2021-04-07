#include<iostream>
#include<string>
#include<cstdlib>

#define TIXML_USE_TICPP
#include "tinyxml.h"
#include "ticpp.h"

int main()
{
	public class car
	{
		bool forward;
		bool backward;
		bool right;
		bool left;
		bool brake;
	};
	car c;
	try
	{
		std::string filename = "config.xml";
		ticpp::Document xmlfile;
		ticpp::Declaration* pheader = new ticpp::Declaration("1.0", "utf-8", "");
		xmlfile.LinkEndChild(pheader);
		ticpp::Elemnet* pSettingElement = new ticpp::Element("Settings");
		ticpp::Element* pElement = new ticpp::Comments("Hello world");
		Element->SetAttribute(c.forward, "forward");
		Element->SetAttribute(c.backward, "backward");
		Element->SetAttribute(c.right, "right");
		Element->SetAttribute(c.left, "left");
		Element->SetAttribute(c.brake, "brake");
		pSettingElement->LinkEndChild(pElement);
		xmlfile.LinkEndChild(pSettingElement);
		xmlfile.SaveFile(filename);
	}
	catch (ticpp::Exception& error)
	{
		return EXIT_FAILURE;
	}

	try
	{
		ticpp::Document xmlFile;
		xmlfile.LoadFile(filename);
		ticpp::Element* pRootElement = xmlFile.FirstChildElement("Settings", false);
		if (pRootElement != 0)
		{
			ticpp::Element* pVideoElement = pRootElement->FirstChildElement("Car", false);
			if (pElement != 0)
			{
				pElement->GetAttribute("forward", c.forward, false);
				pElement->GetAttribute("backward", c.backward, false);
				pElement->GetAttribute("right", c.right, false);
				pElement->GetAttribute("left", c.left, false);
				pElement->GetAttribute("brake", c.brake, false);
			}
		}
		std::cout << "Forward" << c.forward;
		std::cout << "Backward" << c.backward;
		std::cout << "Right" << c.right;
		std::cout << "Left" << c.left;
		std::cout << "Brake" << c.brake;
	}
	catch (ticpp::Excpetion& error)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}