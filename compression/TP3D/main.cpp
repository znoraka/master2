#include <string>
#include <fstream>
#include <math.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <stdio.h>
#include <string.h>

class Point {
public:
  Point(double x, double y, double z, bool cart = true) {
    set(x, y, z, cart);
  }

  void set(double x, double y, double z, bool cart = true) {
    if(cart) {
      this->x = x;
      this->y = y;
      this->z = z;

      this->r = sqrt(x*x + y*y + z*z);
      this->theta = atan2(y, x);
      this->phi = acos(z / r);
      
    } else {
      this->r = x;
      this->theta = y;
      this->phi = z;

      this->x = r * cos(theta) * sin(phi);
      this->y = r * sin(theta) * sin(phi);
      this->z = r * cos(phi);
    }
  }
  
  double x, y, z;
  double r, theta, phi;
};

class Bin {
public:
  Bin(double rmin, double rmax) {
    this->rmin = rmin;
    this->rmax = rmax;
  }

  void addPoint(Point *p) {
    points.push_back(p);
  }

  void normalize() {
    double norme = rmax - rmin;
    for(auto p : points) {
      double r = (p->r - rmin) / norme;
      p->set(r, p->theta, p->phi, false);
    }
  }

  void normalizeInv() {
    double norme = rmax - rmin;
    for(auto p : points) {
      double r = p->r * norme + rmin;
      p->set(r, p->theta, p->phi, false);
    }
  }

  void insertMessage(bool message, double alpha, double delta) {
    normalize();

    auto binAvg = [=] () {
      double avg = 0;
      for(auto i : points) {
	avg += i->r;
      }
      return avg / (double) points.size();
    };

    double d = 0.01 * (double) points.size();
    float k = 1;
    while(binAvg() > 0.5 + (message?1:-1) * alpha != message) {
      for(auto i : points) {
    	i->set(pow(i->r, k) , i->theta, i->phi, false);
      }
      k += (message?-1:1) * delta;
    }
    normalizeInv();

    // std::cout << message << "" << readMessage() << std::endl;
  }

  bool readMessage() {
    normalize();

    auto binAvg = [=] () {
      double avg = 0;
      for(auto i : points) {
	avg += i->r;
      }
      return avg / (double) points.size();
    };
    return binAvg() > 0.5;
  }

  double rmin, rmax;
  std::vector<Point*> points;
};

class Face {
public:
  Face(Point *p1, Point *p2, Point *p3) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
  }
  
  Point *p1, *p2, *p3;
};

class Maillage {
public:
  Maillage() {}

  void exportMaillage(std::string path) {
    std::ofstream file(path);
    file << "OFF\n";
    file << points.size() << " " << indexes.size() << " " << 0;
    for(auto i : points) {
      file << "\n" << i->x << " " << i->y << " " << i->z;
    }
    for(auto i : indexes) {
      file << "\n" << i[0] << " " << i[1] << " " << i[2] << " " << i[3];
    }
    file.close();
  }
  
  void lireMaillage(std::string path) {
    std::ifstream f(path.c_str());

    std::string line;
    std::getline(f, line);
    std::getline(f, line);

    std::istringstream iss(line);

    int nbCoords;
    int nbFaces;
    // std::vector<Point *> points;

    iss >> nbCoords >> nbFaces;

    long cpt = 0;

    while (std::getline(f, line)) {
      std::istringstream i(line);

      if(cpt++ < nbCoords) {
	double x, y, z;
	if (!(i >> x >> y >> z)) { 
	  break; 
	}
        points.push_back(new Point(x, y, z));
      } else {
	int a, x, y, z;
	if (!(i >> a >> x >> y >> z)) { 
	  break; 
	}
	std::vector<int> vec;
	vec.push_back(a); vec.push_back(x); vec.push_back(y); vec.push_back(z);
	indexes.push_back(vec);
	faces.push_back(new Face(points[x], points[y], points[z]));
      }
    }
  }

  void center(Point *c) {
    for(auto p : points) {
      p->set(p->x - c->x, p->y - c->y, p->z - c->z);
    }
  }

  Point *calculG() {
    Point *p = new Point(0, 0, 0);

    for(auto point : points) {
      p->x += point->x;
      p->y += point->y;
      p->z += point->z;
    }
    p->x /= points.size();
    p->y /= points.size();
    p->z /= points.size();
    return p;
  }

  std::vector<bool> readMessage(int binCount) {
    std::vector<bool> message;
    
    Point *p = calculG();
    this->center(p);

    std::vector<Bin*> bins;
    double rmin = points[0]->r;
    double rmax = points[0]->r;

    for(auto p : points) {
      if(p->r > rmax) rmax = p->r;
      if(p->r < rmin) rmin = p->r;
    }
    double step = (rmax - rmin) / (double) binCount;
  
    for (int i = 0; i < binCount; i++) {
      bins.push_back(new Bin(step * i + rmin, step * (i+1) + rmin));
    }

    for(auto p : points) {
      bins[fmin(binCount - 1, (p->r - rmin) / step)]->addPoint(p);
    }

    for (int i = 0; i < binCount; i++) {
      message.push_back(bins[i]->readMessage());
    }
    
    return message;
  }

  void insertMessage(std::vector<bool> message, double alpha, double delta) {
    Point *p = calculG();
    this->center(p);

    int n = message.size();
    std::vector<Bin*> bins;
    double rmin = points[0]->r;
    double rmax = points[0]->r;

    for(auto p : points) {
      if(p->r > rmax) rmax = p->r;
      if(p->r < rmin) rmin = p->r;
    }
    double step = (rmax - rmin) / (double) n;
  
    for (int i = 0; i < n; i++) {
      bins.push_back(new Bin(step * i + rmin, step * (i+1) + rmin));
    }

    for(auto p : points) {
      // std::cout << (p->r - rmin) / step << std::endl;
      bins[fmin(n - 1, (p->r - rmin) / step)]->addPoint(p);
    }

    for (int i = 0; i < n; i++) {
      bins[i]->insertMessage(message[i], alpha, delta);
    }
  }
  
  std::vector<Point *> points;
  std::vector<std::vector<int>> indexes;
  std::vector<Face *> faces;
};

int main(int argc, char *argv[]) {

  auto f = [=](std::string s) {
    for (int i = 0; i < argc; i++) {
      if(strcmp(argv[i], s.c_str()) == 0) {
	return i + 1;
      }
    }
    return -1;
  };

  auto parseMessage = [](std::string s) {
    std::vector<bool> vec;
    for (int i = 0; i < s.size(); i++) {
      vec.push_back(s[i] == '1');
    }
    return vec;
  };
  
  if(argc == 1) {
    std::cout << "encoding : " << std::endl;
    std::cout << "\t-i <input file>" << std::endl;
    std::cout << "\t-o <output file> [default ./out.off]" << std::endl;
    std::cout << "\t-m <binary message>" << std::endl;
    std::cout << "\t-a <alpha> [default 0.1]" << std::endl;
    std::cout << "\t-d <delta> [default 0.9]" << std::endl;

    std::cout << "decoding : " << std::endl;
    std::cout << "\t-r <file to decode>" << std::endl;
    std::cout << "\t-l <message length>" << std::endl;
    
  }
  
  if(f("-r") == -1) {
    char *inputFile = argv[f("-i")];
    const char *outputFile = ((f("-o") == -1)?std::string("./out.off").c_str():argv[f("-o")]);
    auto message = parseMessage(argv[f("-m")]);
    double alpha = ((f("-a") == -1)?0.1:std::stod(argv[f("-a")]));
    double delta = ((f("-d") == -1)?0.9:std::stod(argv[f("-d")]));

    Maillage m;
    m.lireMaillage(inputFile);
    m.insertMessage(message, alpha, delta);
    m.exportMaillage(outputFile);
    
  } else {
    
    Maillage m;
    m.lireMaillage(argv[f("-r")]);
    
    auto res = m.readMessage(std::atoi(argv[f("-l")]));
    for(auto i : res) {
      std::cout << i;
  
    }
    std::cout << std::endl;
  }
  // int cpt = 0;
  // for (int i = 0; i < bins.size(); i++) {
  //   cpt += bins[i].points.size();
  //   std::cout << i << " " << bins[i].points.size() << std::endl;
  // }

  

  return 0;
}
