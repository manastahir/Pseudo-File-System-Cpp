#include <iostream>
#include <vector> 
#include<fstream>
#include<sstream>

using namespace std;

class File
{
private:
	vector<int> pages;
	string fileName;
	int ptr;

public:
	File(string fname)
	{
		this->fileName = fname;
		ptr = 0;
	}

	void rename(string newName)
	{
		this->fileName = newName;
	}

	void append(int pageno)
	{
		for (int i = 0; i < pages.size(); i++)
			if (this->pages[i] == pageno)
				return;
		pages.push_back(pageno);
	}

	void delPages(int pageno)
	{
		int chunkid = -1;
		for (int i = 0; i < this->pages.size(); i++)
			if (this->pages[i] == pageno)
				chunkid = i;
		this->pages.erase(this->pages.begin() + chunkid);
	}

	void setPtr(int point)
	{
		this->ptr = point;
	}

	void writeFile(fstream& file)
	{
		file << this->fileName << endl;
		file << this->ptr << endl;
		file << this->pages.size() << endl;
		for (auto i = this->pages.begin(); i != this->pages.end(); i++)
			file << *i << endl;
	}

	void readFile(fstream& fin)
	{
		getline(fin,this->fileName);

		string num;
		getline(fin, num);
		this->ptr =  stoi(num);

		getline(fin, num);
		int n = stoi(num);
		
		string pg_num;
		for (int i = 0; i < n; i++)
		{
			getline(fin, pg_num);
			this->pages.push_back(stoi(pg_num));
		}
	}

	vector<int> getPages()
	{
		return this->pages;
	}

	string getName()
	{
		return this->fileName;
	}

	int getEndFilePtr()
	{
		return this->ptr;
	}
};

class Dir
{
private:
	vector<File*> files;
	vector<Dir*> child;
	string name;

public:
	Dir(string dirName)
	{
		this->name = dirName;
	}

	void addFile(File* f)
	{
		this->files.push_back(f);
	}

	void deleteFile(string fileName)
	{
		int index = 0;
		for (auto i = this->files.begin(); i != this->files.end(); i++, index++)
			if ((*i)->getName() == fileName)
			{
				files.erase(this->files.begin() + index);
				return;
			}
	}

	void addSubDir(Dir* dir)
	{

		this->child.push_back(dir);
	}

	void deleteSubDir(string subName)
	{
		int index = 0;
		for (auto i = this->child.begin(); i != this->child.end(); i++, index++)
			if ((*i)->getName() == subName)
			{
				child.erase(this->child.begin() + index);
				return;
			}
	}

	vector<File*> getFiles()
	{
		return this->files;
	}

	vector<Dir*> getChildren()
	{
		return this->child;
	}

	string getName()
	{
		return this->name;
	}
};

class Tree
{
private:
	Dir* root, *curr;
	string path;
public:
	Tree()
	{
		root = curr = new Dir("root");
		path.append("root:\\");
	}

	void createDir(string dirName)
	{
		vector<Dir*> dirs = this->curr->getChildren();
		for (int i = 0; i < dirs.size(); i++)
			if (dirs[i]->getName().compare(dirName) == 0)
			{
				cout << "Direcory alread exists" << endl;
				return;
			}
		Dir* dir = new Dir(dirName);
		curr->addSubDir(dir);
	}

	void changeDir(string dirName)
	{
		vector<Dir*> children = curr->getChildren();

		for (auto i = children.begin(); i != children.end(); i++)
			if ((*i)->getName() == dirName)
			{
				curr = (*i);
				path.append(curr->getName() + "\\");
				break;
			}
	}

	void addFile(File* file)
	{
		this->curr->addFile(file);
	}

	File* getFile(string fileName)
	{
		vector<File*> files = curr->getFiles();

		for (auto i = files.begin(); i != files.end(); i++)
			if ((*i)->getName().compare(fileName) == 0)
				return (*i);
			
		
		return NULL;
	}

	void deleteFile(string fileName)
	{
curr->deleteFile(fileName);
	}

	void writeTree(fstream& fout)
	{
		writetraverse(root, fout);
	}

	void readTree(fstream& fin)
	{
		this->root = readDir(fin);
		readCreateTree(root, fin);
		this->curr = this->root;
	}

	bool fileExists(string fname)
	{
		vector<File*> files = this->curr->getFiles();
		for (int i = 0; i < files.size(); i++)
			if (files[i]->getName().compare(fname) == 0)
				return true;

		return false;
	}

	string getPath()
	{
		return this->path;
	}

	vector<File*> getCurrFiles()
	{
		return curr->getFiles();
	}

	void memoryMap()
	{

		levelOrder(root, "");
	}
	
private:
	void writetraverse(Dir* current, fstream& fout)
	{
		fout << current->getName() << endl;
		fout << current->getFiles().size() << endl;

		vector<File*> files = current->getFiles();

		for (auto file = files.begin(); file != files.end(); file++)
			(*file)->writeFile(fout);

		fout << current->getChildren().size() << endl;
		vector<Dir*> children = current->getChildren();
		for (auto child = children.begin(); child != children.end(); child++)
			writetraverse(*child, fout);
	}

	void readCreateTree(Dir* current, fstream& fin)
	{
		string numChild;
		getline(fin, numChild);
		int n = stoi(numChild);

		for (int i = 0; i < n; i++)
		{
			Dir* child = readDir(fin);
			current->addSubDir(child);
			readCreateTree(child, fin);
		}
	}

	Dir* readDir(fstream& fin)
	{
		string dirName;
		getline(fin, dirName);

		Dir* current = new Dir(dirName);

		string numFiles;
		getline(fin, numFiles);
		int n = stoi(numFiles);

		for (int i = 0; i < n; i++)
		{
			File* file = new File(" ");
			file->readFile(fin);
			current->addFile(file);
		}
		return current;
	}

	void levelOrder(Dir* current, string indent)
	{
		cout << indent << "./" << current->getName()<< endl;

		vector<File*> files = current->getFiles();

		for (auto i = files.begin(); i != files.end(); i++)
		{
			cout << indent << indent << (*i)->getName() << " ";
			vector<int> pages = (*i)->getPages();

			for (auto j = pages.begin(); j != pages.end(); j++)
				cout << (*j) << "->";
			cout << "x" << endl;
		}


		vector<Dir*> children = current->getChildren();

		for (auto i = children.begin(); i != children.end(); i++)
			levelOrder(*i, indent + " ");
	}
};

class FileManagement
{
private:
	int TOTAL_PAGES, PAGE_SIZE;
	bool* pagetable;

public:
	FileManagement(int total_pages, int page_size, bool* pagetable)
	{
		this->TOTAL_PAGES = total_pages;
		this->pagetable = pagetable;
		this->PAGE_SIZE = page_size;
	}

	vector<int> write(string txt, fstream& datFile, vector<int> pages, int offset)
	{
		if (offset == 16)
		{
			pages.push_back(nextAvailablePage());
			offset = 0;
		}

		int bytesRemaining = txt.length();
		int bytesToWrite = this->PAGE_SIZE - offset;
		int stringPos = 0;

		if (pages.size() == 0)
			pages.push_back(this->nextAvailablePage());

		while (bytesRemaining > bytesToWrite)
		{
			//seek to page + offset
			int page = pages.at(pages.size() - 1);
			datFile.seekp(page * this->PAGE_SIZE + offset);
			//write upto pagesize - offset
			datFile << txt.substr(stringPos, bytesToWrite);

			//cout << "Page " << page << endl;
			//cout << "Text " << txt.substr(stringPos, bytesToWrite) << endl;

			stringPos += bytesToWrite;
			bytesRemaining -= bytesToWrite;
			pages.push_back(nextAvailablePage());
			offset = 0;
			bytesToWrite = this->PAGE_SIZE;
		}

		int pg = pages.at(pages.size() - 1);
		datFile.seekp(pg * this->PAGE_SIZE + offset);
		datFile << txt.substr(stringPos, bytesRemaining);

		//cout << "Page " << pg << endl;
		//cout << "Text " << txt.substr(stringPos, bytesRemaining) << endl;

		offset += bytesRemaining;
		//cout << "Offset " << offset << endl;

		pages.push_back(-1);
		pages.push_back(offset);

		return pages;
	}

	string read(fstream& datFile, vector<int>pages, int offset)
	{
		stringstream ss;
		char s;

		for (auto i = pages.begin(); i != pages.end(); i++)
		{
			if (*i == pages[pages.size() - 1])
				break;

			datFile.seekg(*i * this->PAGE_SIZE);
			for (int j = 0; j < this->PAGE_SIZE; j++)
			{
				datFile.get(s);
				ss << s;
			}
		}

		datFile.seekg(pages[pages.size() - 1] * this->PAGE_SIZE);
		for (int i = 0; i < offset; i++)
		{
			datFile.get(s);
			ss << s;
		}
		return ss.str();
	}

	vector<int> truncate(int size, vector<int> pages, int offset)
	{
		vector<int> info;
		int max = (pages.size() - 1) * this->PAGE_SIZE + offset;
		if (size >= max)
			return info;

		int count = size / this->PAGE_SIZE;
		int limit = size % this->PAGE_SIZE;
		int totalPages = pages.size();

		if (count + 1 < pages.size())
		{
			for (int i = 0; i < totalPages - count - 1; i++)
			{
				this->pagetable[pages.back()] = false;
				info.push_back(pages.back());
				pages.pop_back();
			}
		}

		offset = limit;
		info.push_back(-1);
		info.push_back(offset);

		return info;
	}

	string readFrom(fstream& datFile,int start, int size, vector<int> pages, int offset)
	{
		int max = (pages.size() - 1) * this->PAGE_SIZE + offset;
		if (start > max || (start + size) > max)
		{
			return "Start + Size exceedes file size!";
		}

		int startingPage = pages[start / this->PAGE_SIZE];
		int endingPage = pages[(start + size) / this->PAGE_SIZE];
		int count = endingPage - startingPage - 1;

		stringstream ss;
		char s;

		datFile.seekg(startingPage * this->PAGE_SIZE + start);

		if (count < 0)
		{
			for (int i = start; i < start + size; i++)
			{
				datFile.get(s);
				ss << s;
			}
		}
		else
		{
			for (int i = start; i < this->PAGE_SIZE - start; i++)
			{
				datFile.get(s);
				ss << s;
			}

			for (int j = 0; j < count; j++)
			{
				datFile.seekg(pages[j] * this->PAGE_SIZE);
				for (int k = 0; k < this->PAGE_SIZE; k++)
				{
					datFile.get(s);
					ss << s;
				}
			}

			datFile.seekg(endingPage * this->PAGE_SIZE);
			for (int l = 0; l < (start + size) % this->PAGE_SIZE; l++)
			{
				datFile.get(s);
				ss << s;
			}
		}

		return ss.str();
	}

	vector<int> writeAt(fstream& datFile, string txt, vector<int> pages, int offset, int pos)
	{
		vector<int> info = pages;
		int max = (pages.size() - 1) * this->PAGE_SIZE + offset;
		int l = txt.length();

		if (pos > max)
			return pages;

		if (l - (max - pos) > 0)
			info = write(txt.substr(max - pos, -1),datFile, pages, offset);

		int startPage = pages[pos / this->PAGE_SIZE];
		int limit = pos % this->PAGE_SIZE;
		datFile.seekp(startPage * this->PAGE_SIZE + limit);
		datFile << txt.substr(0, this->PAGE_SIZE - limit);

		int currentpage = startPage;

		int count = (l - this->PAGE_SIZE + limit) / this->PAGE_SIZE;
		int strStart = this->PAGE_SIZE - limit;
		for (int i = 1; i <= count; i++)
		{
			datFile.seekp(pages[i] * this->PAGE_SIZE);
			datFile << txt.substr(strStart, this->PAGE_SIZE);
			strStart += this->PAGE_SIZE;
			currentpage = pages[i];
		}

		if (l > this->PAGE_SIZE - limit)
		{
			datFile.seekp((currentpage + 1) * this->PAGE_SIZE);
			datFile << txt.substr(strStart, max - pos);
		}

		if (l - (max - pos) < 0)
		{
			info.push_back(-1);
			info.push_back(offset);
		}

		return info;
	}


private:
	int nextAvailablePage()
	{
		for (int i = 0; i < this->TOTAL_PAGES; i++)
		{
			if (!this->pagetable[i])
			{
				this->pagetable[i] = true;
				return i;
			}
		}
		return -1;
	}
};

class FileSystem
{
private:
	const int PAGE_SIZE = 16;
	const int TOTAL_PAGES = 1024;
	bool* Pages;
	const string FILE_NAME = "FileSystem.dat";
	fstream DATFILE;
	Tree* tree;
	FileManagement* filemanager;
	int offset;
	int limit;

public:
	FileSystem()
	{
		struct stat buffer;
		this->offset = 0;
		this->Pages = new bool[this->TOTAL_PAGES];

		for (int i = 0; i < this->TOTAL_PAGES; i++)
			this->Pages[i] = false;

		this->tree = new Tree();
		this->filemanager = new FileManagement(this->TOTAL_PAGES, this->PAGE_SIZE, this->Pages);
		this->limit = this->PAGE_SIZE*this->TOTAL_PAGES;

		if (stat(this->FILE_NAME.c_str(), &buffer) == -1)
		{
			this->DATFILE.open(this->FILE_NAME, fstream::out);
			this->DATFILE.close();
			this->DATFILE.open(this->FILE_NAME, fstream::in | fstream::out);
		}
		
		else
		{
			this->DATFILE.open(this->FILE_NAME, fstream::in | fstream::out);
			this->DATFILE.seekg(this->limit);
			this->readPageTable();
			this->tree->readTree(this->DATFILE);
			this->DATFILE.seekg(0);
		}
		
	}

	void createDir(string name)
	{
		this->tree->createDir(name);
	}

	void changeDir(string name)
	{
		this->tree->changeDir(name);
	}

	void createFile(string name, string data = " ")
	{
		if (this->tree->fileExists(name)) return;

		if (data.compare(" ") == 0)
		{
			this->tree->addFile(new File(name));
			return;
		}


		File* file = new File(name);
		vector<int> info = this->filemanager->write(data, this->DATFILE,file->getPages(), 0);
		
		auto i = info.begin();
		for (; i != info.end(); i++)
			if (*i != -1)
				file->append(*i);
			else
				break;

		file->setPtr(*(++i));

		this->tree->addFile(file);
	}

	void delFile(string name)
	{
		if (!this->tree->fileExists(name))
		{
			cout << "File does not exist" << endl;
			return;
		}

		vector<int> pgs = this->tree->getFile(name)->getPages();

		for (auto i = pgs.begin(); i != pgs.end(); i++)
			this->Pages[*i] = false;

		this->tree->deleteFile(name);
	}

	void readFile(string name, int start = 0, int size = 0)
	{
		if (!this->tree->fileExists(name))
		{
			cout << "No scuh file Exists" << endl;
			return;
		}

		File* file = this->tree->getFile(name);

		vector<int> pages = file->getPages();
		int offset = file->getEndFilePtr();

		if (start == 0 && size == 0)
			cout << this->filemanager->read(this->DATFILE, pages, offset) << endl;
		else
			cout << this->filemanager->readFrom(this->DATFILE, start, size, pages, offset) << endl;
	}

	void writeToFile(string fname, string data, int at = 0)
	{
		File* file = this->tree->getFile(fname);
		vector<int> info;
		if (at == 0)
			info = this->filemanager->write(data, this->DATFILE, file->getPages(), file->getEndFilePtr());
		else
			info = this->filemanager->writeAt(this->DATFILE, data, file->getPages(), file->getEndFilePtr(), at);

		auto i = info.begin();
		for (; i != info.end(); i++)
			if (*i != -1)
				file->append(*i);
			else
				break;

		file->setPtr(*(++i));
	}

	void truncate(string fname, int size)
	{
		File* file = this->tree->getFile(fname);
		vector<int> info = this->filemanager->truncate(size, file->getPages(), file->getEndFilePtr());

		auto i = info.begin();
		for (; i != info.end(); i++)
			if (*i != -1)
				file->delPages(*i);
			else
				break;

		file->setPtr(*(++i));
	}

	void showMemoryMap()
	{
		this->tree->memoryMap();
	}

	void listFiles()
	{
		vector<File*> files = this->tree->getCurrFiles();

		for (auto i = files.begin(); i != files.end(); i++)
			cout << (*i)->getName() << endl;
	}

	string path()
	{
		return this->tree->getPath();
	}

	~FileSystem()
	{
		this->offset = this->limit;
		this->DATFILE.seekp(this->offset);
		this->writePageTable();
		this->tree->writeTree(this->DATFILE);
		this->DATFILE.close();
	}


private:
	void writePageTable()
	{
		for (int i = 0; i < this->TOTAL_PAGES; i++)
			DATFILE << this->Pages[i] << endl;
	}

	void readPageTable()
	{
		for (int i = 0; i < this->TOTAL_PAGES; i++)
		{
			string n;
			getline(this->DATFILE, n);
			this->Pages[i] = stoi(n);
		}
	}

};


vector<string> getSubStrings(string s)
{
	vector<string> tokens;
	string delimiter = " ";
	size_t pos = 0;
	string token;

	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		tokens.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	tokens.push_back(s);
	return tokens;
}

int main()
{
	FileSystem* f = new FileSystem();

	string s;
	
	while (1) {
		cout << "<" << f->path() << ">";
		getline(cin, s);

		vector<string> command = getSubStrings(s);

		if (command[0].compare("cd") == 0) {
			f->changeDir(command[1]);
		}

		else if (command[0].compare("mkdir") == 0) {
			f->createDir(command[1]);
		}

		else if (command[0].compare("mkfile") == 0) {
			f->createFile(command[1]);
		}

		else if (command[0].compare("delfile") == 0) {
			f->delFile(command[1]);
		}

		else if (command[0].compare("rdfile") == 0) {
			if (command[1].compare("-o") == 0) {
				f->readFile(command[2]);
			}
			else if (command[1].compare("-of") == 0) {
				int offset;
				int size;
				cout << "Offset: ";
				cin >> offset;
				cout << "Size: ";
				cin >> size;
				cin.ignore();
				f->readFile(command[2], offset, size);
			}
			else
				cout << "Invalid Mode" << endl;

		}

		else if (command[0].compare("trfile") == 0) {
			int size;
			cout << "Size: ";
			cin >> size;
			cin.ignore();
			f->truncate(command[1], size);
		}

		else if (command[0].compare("wrtfile") == 0) {
			if (command[1].compare("-o") == 0) {
				string data;
				getline(cin, data);
				f->writeToFile(command[2], data);
			}
			else if (command[1].compare("-of") == 0) {
				int offset;
				string data;
				cout << "Offset: ";
				cin >> offset;
				cin.ignore();
				getline(cin, data);
				f->writeToFile(command[2], data, offset);
			}
			else
				cout << "Invalid Mode" << endl;

		}

		else if (command[0].compare("ls") == 0) {
			f->listFiles();
		}

		else if (command[0].compare("map") == 0) {
			f->showMemoryMap();
		}

		else if (command[0].compare("exit") == 0) {
			break;
		}

		else if (command[0].compare("help") == 0) {
			cout << "Commands provided are:" << endl;
			cout << "CD\tDisplays the name of or changes the current directory." << endl;
			cout << "MKDIR\tCreates a directory." << endl;
			cout << "MKFILE\tCreates a file in the directory." << endl;
			cout << "DELFILE\tDeletes the file." << endl;
			cout << "RDFILE\tReads the file" << endl;
			cout << "TRFILE\tTruncates the files" << endl;
			cout << "WRTFILE\tWrites to a file." << endl;
			cout << "LS\tList all the files" << endl;
			cout << "MAP\tShows the memory mapping." << endl;
			cout << "CLS\tClear the screen" << endl;
			cout << "EXIT\tExit the program" << endl;
		}
		else if (command[0].compare("cls") == 0)
			system("cls");
		else
			cout << "'" << command[0] << "'" << " is not recognized as an internal or external command, operable program or batch file." << endl;

	}
	f->~FileSystem();


	//system("cls");
	


	/* f->createDir("Dir 1");
	f->changeDir("Dir 1");
	
	f->createFile("Anas", "Hello my name is Anas");
	f->createFile("Ahsan", "Hello my name is Ahsan");
	f->createFile("Muhammad", "Hello my name is Muhammad Bin Jamil");
	f->createFile("Haseeb", "Hello my name is Haseeb");
	
	f->writeToFile("Anas", "I am Anas Tahir");
	
	f->delFile("Anas");
	f->showMemoryMap();
	
	f->createFile("Anas Tahir", "I am Anas Tahir");
	f->writeToFile("Anas Tahir", " version 2");
	f->showMemoryMap();
	
	f->readFile("Anas");
	f->readFile("Anas Tahir");
	f->readFile("Haseeb");
	
	//f->truncate("Muhammad", 25);
	f->writeToFile("Muhammad", "I am soota", 10);
	f->readFile("Muhammad", 0, 25);

	f->showMemoryMap();


	f->~FileSystem(); */
}


