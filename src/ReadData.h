#include<vector>

//
// reads delimited data from file; returns data as vector of vectors
//
template <class T>
bool readDataFromFile(std::vector< vector<T> >& data, string fileName, uint offset = 0, uint skip = 1)
{
	ifstream file(fileName.c_str());
	if(!file)
		return false;

	data.clear();
	string line;
	uint instance = 0;
	uint sinceLastSkip = 0;

	// offset
	for(uint i = 0; i < offset; i++)
		getline(file, line);

	while(getline(file, line)) {
		// skip
		sinceLastSkip++;
		if(sinceLastSkip < skip)
			continue;
		sinceLastSkip = 0;

		vector<T> set;
		data.push_back(set);
		stringstream ss(line);
		T datum;
		while(ss >> datum)
			data[instance].push_back(datum);
		instance++;
	}

	return true;
}
