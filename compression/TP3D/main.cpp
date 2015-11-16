#include <string>
#include <fstream>
#include <math.h>
#include <sstream>
#include <vector>
#include <iostream>

class Point {
public:
  Point(float x, float y, float z, bool cart = true) {
    set(x, y, z, cart);
  }

  void set(float x, float y, float z, bool cart = true) {
    if(cart) {
      this->x = x;
      this->y = y;
      this->z = z;

      this->r = sqrt(x*x + y*y + z*z);
      this->phi = atan(y / x);
      this->theta = acos(z / r);
      
    } else {
      this->r = x;
      this->theta = y;
      this->phi = z;

      this->x = r * sin(theta) * cos(phi);
      this->y = r * sin(theta) * sin(phi);
      this->z = r * cos(theta);
    }
  }
  
  float x, y, z;
  float r, theta, phi;
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
  
  std::vector<Point *> points;
  std::vector<std::vector<int>> indexes;
  std::vector<Face *> faces;
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
    float norme = rmax - rmin;
    for(auto p : points) {
      float r = p->r / norme;
      p->set(r, p->theta, p->phi, false);
    }
  }

  void normalizeInv() {
    float norme = rmax - rmin;
    for(auto p : points) {
      float r = p->r * norme;
      p->set(r, p->theta, p->phi, false);
    }
  }

  void insertMessage(int message) {
    //TOOD
  }

  double rmin, rmax;
  std::vector<Point*> points;
};


int main() {
  Maillage m;
  m.lireMaillage("../TP3D/bunny.off");
  // std::cout << m.calculG()->x << ", " << m.calculG()->y << ", " << m.calculG()->z << std::endl;

  Point *p = m.calculG();

  m.center(p);

  int n = 16;
  std::vector<Bin> bins;
  float rmin = m.points[0]->r;
  float rmax = m.points[0]->r;

  for(auto p : m.points) {
    if(p->r > rmax) rmax = p->r;
    if(p->r < rmin) rmin = p->r;
  }
  
  float step = (rmax - rmin) / (float) n;
  // std::cout << step << std::endl;
  
  for (int i = 0; i < n; i++) {
    bins.push_back(Bin(step * i - rmin, step * (i+1) - rmin));
  }

  for(auto p : m.points) {
    // std::cout << (p->r - rmin) / step << std::endl;
    bins[(p->r - rmin) / step].addPoint(p);
  }

  int cpt = 0;
  for (int i = 0; i < bins.size(); i++) {
    cpt += bins[i].points.size();
    std::cout << i << " " << bins[i].points.size() << std::endl;
  }

  // std::cout << cpt << std::endl;
  return 0;
}
