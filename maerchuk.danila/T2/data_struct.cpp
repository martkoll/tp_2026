#include "data_struct.hpp"
#include "iofmtguard.hpp"
#include <iomanip>

namespace nspace
{
  // Парсинг разделителя
  std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c = '0';
    in >> c;
    if (in && (std::tolower(c) != std::tolower(dest.exp))) in.setstate(std::ios::failbit);
    return in;
  }

  // CHR LIT: 'a' [cite: 68]
  std::istream& operator>>(std::istream& in, CharIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    return in >> DelimiterIO{ '\'' } >> dest.ref >> DelimiterIO{ '\'' };
  }

  // DBL SCI: 1.0e+2 [cite: 41]
  std::istream& operator>>(std::istream& in, DoubleSciIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    // Используем стандартный манипулятор scientific для чтения
    if (!(in >> dest.ref)) in.setstate(std::ios::failbit);
    return in;
  }

  std::istream& operator>>(std::istream& in, StringIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
  }

  // Чтение DataStruct [cite: 24, 25, 27]
  std::istream& operator>>(std::istream& in, DataStruct& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct input;
    bool keys_found[3] = {false, false, false};

    in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' };
    for (int i = 0; i < 3; ++i)
    {
      std::string key;
      std::getline(in, key, ' ');
      if (key == "key1") {
        in >> CharIO{ input.key1 };
        keys_found[0] = true;
      } else if (key == "key2") {
        in >> DoubleSciIO{ input.key2 };
        keys_found[1] = true;
      } else if (key == "key3") {
        in >> StringIO{ input.key3 };
        keys_found[2] = true;
      }
      in >> DelimiterIO{ ':' };
    }
    in >> DelimiterIO{ ')' };

    if (in && keys_found[0] && keys_found[1] && keys_found[2]) {
      dest = std::move(input);
    } else {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  // Компаратор [cite: 6, 7, 8, 9]
  bool DataStruct::operator<(const DataStruct& other) const
  {
    if (key1 != other.key1) return key1 < other.key1;
    if (key2 != other.key2) return key2 < other.key2; // Для double scientific сравнение прямое
    return key3.length() < other.key3.length();
  }

  // Вывод [cite: 12, 45, 81, 82]
  std::ostream& operator<<(std::ostream& out, const DataStruct& src)
  {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;
    iofmtguard fmtguard(out);
    out << "(:key1 '" << src.key1 << "':key2 "
        << std::scientific << std::setprecision(1) << src.key2 // 1.0e+01 [cite: 45, 81]
        << ":key3 \"" << src.key3 << "\":)";
    return out;
  }
}
