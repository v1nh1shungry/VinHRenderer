#include "initializer.h"
#include "gl.h"
#include "win32.h"
#include "log.h"

#include <iomanip>

int main(int argc, char** argv)
{
	using namespace std;
	using namespace vinh;

	if (argc < 2)
	{
		ERRORING("Expect an ini file.");
		system("pause");
		return 1;
	}

	Camera camera({ 0.f, 0.f, 3.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f });
	Light light;
	Rasterizer rst;
	Initializer initializer(argv[1]);
	initializer.setShader.connect(&rst, &Rasterizer::setShader);
	initializer.setCamera.connect(&camera, &Camera::setup);
	initializer.setLight.connect(&light, &Light::setup);
	auto models = initializer.parse();
	auto title = initializer.title();
	auto ui = UI::getInstance(title.c_str());
	rst.drawFrame.connect(ui, &UI::draw);
	ui->mouseEvents.connect(&camera, &Camera::update);
	ui->setShader.connect(&rst, &Rasterizer::setShader);

	int frames = 0;
	float prevTime = ui->getSysTime();
	while (ui->run())
	{
		float currTime = ui->getSysTime();
		rst.draw(models, camera, light);

		++frames;
		if (currTime - prevTime >= 1)
		{
			int sumMills = (int)(currTime - prevTime) * 1000;
			int avgMills = sumMills / frames;
			LOGGING("fps: ", setw(3), frames, ", avg: ", setw(3), avgMills, " ms");
			frames = 0;
			prevTime = currTime;
		}
		rst.clear();
	}

	for (auto& model : models)
	{
		delete model;
	}

	return 0;
}