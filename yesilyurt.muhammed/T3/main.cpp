#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <set>

struct Point
{
  int x;
  int y;
};

struct Polygon
{
  std::vector< Point > points;
};

std::istream& operator>>(std::istream& in, Point& point)
{
  char left = '\0';
  char semicolon = '\0';
  char right = '\0';

  in >> left >> point.x >> semicolon >> point.y >> right;

  if (left != '(' || semicolon != ';' || right != ')')
  {
    in.setstate(std::ios::failbit);
  }

  return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly)
{
  std::size_t count = 0;
  in >> count;

  if (!in || count < 3)
  {
    in.setstate(std::ios::failbit);
    return in;
  }

  poly.points.clear();

  std::set< std::pair< int, int > > uniquePoints;

for (std::size_t i = 0; i < count; ++i)
{
  Point p;
  in >> p;

  if (!in)
  {
    in.setstate(std::ios::failbit);
    return in;
  }

  if (uniquePoints.count({ p.x, p.y }) > 0)
  {
    in.setstate(std::ios::failbit);
    return in;
  }

  uniquePoints.insert({ p.x, p.y });

  poly.points.push_back(p);
}

return in;
}

double getArea(const Polygon& poly)
{
  double area = 0.0;

  for (std::size_t i = 0; i < poly.points.size(); ++i)
  {
    std::size_t j = (i + 1) % poly.points.size();
    area += poly.points[i].x * poly.points[j].y;
    area -= poly.points[j].x * poly.points[i].y;
  }

  return std::abs(area) / 2.0;
}

bool isInFrame(const std::vector< Polygon >& polygons, const Polygon& target)
{
  if (polygons.empty())
  {
    return false;
  }

  int minX = polygons[0].points[0].x;
  int maxX = polygons[0].points[0].x;
  int minY = polygons[0].points[0].y;
  int maxY = polygons[0].points[0].y;

  for (const Polygon& poly : polygons)
  {
    for (const Point& p : poly.points)
    {
      minX = std::min(minX, p.x);
      maxX = std::max(maxX, p.x);
      minY = std::min(minY, p.y);
      maxY = std::max(maxY, p.y);
    }
  }

  for (const Point& p : target.points)
  {
    if (p.x < minX || p.x > maxX || p.y < minY || p.y > maxY)
    {
      return false;
    }
  }

  return true;
}
bool arePerms(const Polygon& a, const Polygon& b)
{
  if (a.points.size() != b.points.size())
  {
    return false;
  }

  std::vector< std::pair< int, int > > first;
  std::vector< std::pair< int, int > > second;

  for (const Point& p : a.points)
  {
    first.push_back({ p.x, p.y });
  }

  for (const Point& p : b.points)
  {
    second.push_back({ p.x, p.y });
  }

  std::sort(first.begin(), first.end());
  std::sort(second.begin(), second.end());

  return first == second;
}

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "ERROR\n";
    return 1;
  }

  std::ifstream file(argv[1]);

  if (!file)
  {
    std::cerr << "ERROR\n";
    return 1;
  }

  std::vector< Polygon > polygons;

  while (!file.eof())
  {
    Polygon poly;

    if (file >> poly)
    {
      polygons.push_back(poly);
    }
    else
    {
      file.clear();
      std::string trash;
      std::getline(file, trash);
    }
  }

  std::cout << std::fixed << std::setprecision(1);

  std::string command;

  while (std::cin >> command)
  {
    if (command == "AREA")
    {
      std::string option;
      std::cin >> option;

      double result = 0.0;

      if (option == "EVEN")
      {
        for (const Polygon& poly : polygons)
        {
          if (poly.points.size() % 2 == 0)
          {
            result += getArea(poly);
          }
        }
        std::cout << result << "\n";
      }
      else if (option == "ODD")
      {
        for (const Polygon& poly : polygons)
        {
          if (poly.points.size() % 2 == 1)
          {
            result += getArea(poly);
          }
        }
        std::cout << result << "\n";
      }
      else if (option == "MEAN")
      {
        if (polygons.empty())
        {
          std::cout << "<INVALID COMMAND>\n";
          continue;
        }

        for (const Polygon& poly : polygons)
        {
          result += getArea(poly);
        }

        std::cout << result / polygons.size() << "\n";
      }
      else
      {
        try
        {
          int vertices = std::stoi(option);

          if (vertices < 3)
          {
            std::cout << "<INVALID COMMAND>\n";
            continue;
          }

          for (const Polygon& poly : polygons)
          {
            if (static_cast<int>(poly.points.size()) == vertices)
            {
              result += getArea(poly);
            }
          }

          std::cout << result << "\n";
        }
        catch (...)
        {
          std::cout << "<INVALID COMMAND>\n";
        }
      }
    }
    else if (command == "MAX")
    {
      std::string option;
      std::cin >> option;

      if (polygons.empty())
      {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }

      if (option == "AREA")
      {
        double maxArea = 0.0;

        for (const Polygon& poly : polygons)
        {
          maxArea = std::max(maxArea, getArea(poly));
        }

        std::cout << maxArea << "\n";
      }
      else if (option == "VERTEXES")
      {
        std::size_t maxVertices = 0;

        for (const Polygon& poly : polygons)
        {
          maxVertices = std::max(maxVertices, poly.points.size());
        }

        std::cout << maxVertices << "\n";
      }
      else
      {
        std::cout << "<INVALID COMMAND>\n";
      }
    }
    else if (command == "MIN")
    {
      std::string option;
      std::cin >> option;

      if (polygons.empty())
      {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }

      if (option == "AREA")
      {
        double minArea = std::numeric_limits< double >::max();

        for (const Polygon& poly : polygons)
        {
          minArea = std::min(minArea, getArea(poly));
        }

        std::cout << minArea << "\n";
      }
      else if (option == "VERTEXES")
      {
        std::size_t minVertices = polygons.front().points.size();

        for (const Polygon& poly : polygons)
        {
          minVertices = std::min(minVertices, poly.points.size());
        }

        std::cout << minVertices << "\n";
      }
      else
      {
        std::cout << "<INVALID COMMAND>\n";
      }
    }
    else if (command == "COUNT")
    {
      std::string option;
      std::cin >> option;

      int count = 0;

      if (option == "EVEN")
      {
        for (const Polygon& poly : polygons)
        {
          if (poly.points.size() % 2 == 0)
          {
            ++count;
          }
        }

        std::cout << count << "\n";
      }
      else if (option == "ODD")
      {
        for (const Polygon& poly : polygons)
        {
          if (poly.points.size() % 2 == 1)
          {
            ++count;
          }
        }

        std::cout << count << "\n";
      }
      else
      {
        try
        {
          int vertices = std::stoi(option);

          for (const Polygon& poly : polygons)
          {
            if (static_cast<int>(poly.points.size()) == vertices)
            {
              ++count;
            }
          }

          std::cout << count << "\n";
        }
        catch (...)
        {
          std::cout << "<INVALID COMMAND>\n";
        }
      }
    }
    else if (command == "INFRAME")
    {
      Polygon target;

      if (std::cin >> target)
      {
        if (isInFrame(polygons, target))
        {
          std::cout << "<TRUE>\n";
        }
        else
        {
          std::cout << "<FALSE>\n";
        }
      }
      else
      {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
        std::string trash;
        std::getline(std::cin, trash);
      }
    }
    else if (command == "PERMS")
{
  Polygon target;

  if (std::cin >> target)
  {
    int count = 0;

    for (const Polygon& poly : polygons)
    {
      if (arePerms(poly, target))
      {
        ++count;
      }
    }

    std::cout << count << "\n";
  }
  else
  {
    std::cout << "<INVALID COMMAND>\n";

    std::cin.clear();

    std::string trash;
    std::getline(std::cin, trash);
  }
}
    else
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  return 0;
}
