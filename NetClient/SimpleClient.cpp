#include <iostream>
#include <olc_net.h>

enum class CustomMsgTypes : uint32_t
{
	FireBullet,
	MovePlayer
};

class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
	public:
		bool FireBullet(float x, float y)
		{
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::FireBullets;
			msg << x << y;
			Sennd(msg);
		}
};

int main()
{
	/*olc::net::message<CustomMsgTypes> msg;
	msg.header.id = CustomMsgTypes::FireBullet;

	int a = 1;
	bool b = true;
	float c = 3.14159f;

	struct
	{
		float x;
		float y;
	} d[5];

	msg << a << b << c << d;

	a = 99;
	b = false;
	c = 99.0f;

	msg >> d >> c >> b >> a;

	return 0;*/

	CustomClient c;
	c.Connect("", 60000);
	c.FireBullet(2.0f, 5.0f);
	return 0
}