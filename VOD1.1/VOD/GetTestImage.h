#include<vector>

class GetTestImage
{
public:
	GetTestImage() {};
	~GetTestImage() {};
	std::vector<std::vector<char>>get_ImageFingerprint(std::string image_path,int mask);
	CString vector_to_CString(std::vector<std::vector<char>> u);

	//int compare(vector<vector<char>>a, vector<vector<char>>b);
};

