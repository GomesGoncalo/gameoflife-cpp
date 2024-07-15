#include "cell.hxx"

#include <boost/program_options.hpp>
#include <iostream>
#include <random>

namespace po = boost::program_options;
namespace boost::conversion::detail {
template <>
bool try_lexical_convert<std::chrono::milliseconds, std::string>(
    const std::string &number, std::chrono::milliseconds &out) {

  char *end;
  double count = std::strtod(number.data(), &end);
  if (number.data() + number.length() != end)
    return false;

  out = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::duration<double, std::milli>(count));

  return true;
}

template <>
bool try_lexical_convert<std::string, std::chrono::milliseconds>(
    const std::chrono::milliseconds &in, std::string &out) {
  out = std::to_string(in.count()) + "ms";
  return true;
}
} // namespace boost::conversion::detail

std::optional<state_data> state_data::create(int argc, const char *argv[]) {
  state_data val;
  po::options_description generic("Generic options");
  generic.add_options()("help", "Produce help message")(
      "width,w", po::value(&val.width)->default_value(val.width), "Width")(
      "height,h", po::value(&val.height)->default_value(val.height),
      "Height")("probability,p",
                po::value(&val.probability)->default_value(val.probability),
                "Life probability")(
      "step,s",
      po::value(&val.logic_granularity)->default_value(val.logic_granularity),
      "Step duration");

  po::options_description visible;
  visible.add(generic);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, visible), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << visible << "\n";
    return std::nullopt;
  }

  val.reset();
  return val;
}

void state_data::reset() {
  board.clear();
  board.reserve(width * height);
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);

  unsigned int idx{0};
  for (auto x = 0u; x < width; ++x) {
    for (auto y = 0u; y < height; ++y) {
      auto prob = dist(rng);
      if (prob >= probability) {
        board.push_back(live{x, y, idx});
      } else {
        board.push_back(dead{x, y, idx});
      }

      ++idx;
    }
  }
}

state_data::state_data(unsigned int width, unsigned height,
                       unsigned probability, std::chrono::milliseconds msec)
    : width{width}, height{height}, probability{probability},
      logic_granularity{std::move(msec)} {
  reset();
}
