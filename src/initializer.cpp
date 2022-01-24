#include "initializer.h"
#include "log.h"

#include <sstream>

using namespace vinh;
using namespace std;

Initializer::Initializer(const char* _filename)
	: filename(_filename)
{
	;
}

// stupid and ugly, however, i dont have better ideas
// maybe a json parser is more beautiful, but boring
vector<Model*> Initializer::parse()
{
	unique_ptr<ifstream, function<void(ifstream*)>> file(new ifstream, [=](ifstream* ptr) {
		LOGGING("Closing ini file '", filename, "' ...");
		ptr->close();
		LOGGING("Finished.");
		delete ptr;
		});
	LOGGING("Loading ini file '", filename, "' ...");
	file->open(filename);
	if (file->fail())
	{
		ERRORING("Can't open ini file '", filename, "'.");
		return {};
	}
	vector<Model*> res;
	std::string line, trash, filename;
	Model* model = nullptr;
	int count = 1;
	bool isMMD = true; // default is true
	LOGGING("Parsing ini file...");
	while (!file->eof())
	{
		getline(*file, line);
		istringstream iss(line);
		if (!line.compare(0, 1, "#"))
		{
			continue;
		}
		else if (!line.compare(0, 6, "model:"))
		{
			if (model) res.push_back(model);
			iss >> trash >> filename;
			model = new Model(filename.c_str(), isMMD);
		}
		else if (!line.compare(0, 5, "diff:"))
		{
			if (!model) {
				ERRORING("line ", count, ": this texture belongs to which model?");
				ERRORING("Loading texture file failed. Just skip it.");
				continue;
			}
			iss >> trash >> filename;
			model->loadTexture(filename.c_str(), Diffuse);
		}
		else if (!line.compare(0, 4, "mmd:"))
		{
			iss >> trash >> isMMD;
			LOGGING("MMD: ", isMMD);
		}
		else if (!line.compare(0, 7, "camera:"))
		{
			char itrash;
			Vector3f res[3];
			iss >> trash;
			for (auto & re : res)
			{
				iss >> re[0] >> itrash >> re[1] >> itrash >> re[2];
			}
			setCamera(res[0], res[1], res[2]);
		}
		else if (!line.compare(0, 7, "shader:"))
		{
			int type;
			iss >> trash >> type;
			LOGGING("Using shader: ", type);
			setShader(type);
		}
		else if (!line.compare(0, 6, "light:"))
		{
			float x, y, z, i;
			iss >> trash >> x >> y >> z >> i;
			setLight(x, y, z, i);
		}
		else if (!line.compare(0, 7, "normal:"))
		{
			if (!model) {
				ERRORING("line ", count, ": this texture belongs to which model?");
				ERRORING("Loading texture file failed. Just skip it");
				continue;
			}
			iss >> trash >> filename;
			model->loadTexture(filename.c_str(), Normal);
		}
		else if (!line.compare(0, 5, "spec:"))
		{
			if (!model) {
				ERRORING("line ", count, ": this texture belongs to which model?");
				ERRORING("Loading texture file failed. Just skip it");
				continue;
			}
			iss >> trash >> filename;
			model->loadTexture(filename.c_str(), Specular);
		}
		else
		{
			iss >> trash;
			ERRORING("line ", count, ": unknown syntax '", trash);
			ERRORING("Just skip it too");
		}
		++count;
	}
	if (model) res.push_back(model);
	return res;
}

string Initializer::title()
{
	string title = filename.substr(0, filename.find('.')) + " - VinHRenderer";
	return title;
}