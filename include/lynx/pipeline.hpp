#ifndef LYNX_PIPELINE_HPP
#define LYNX_PIPELINE_HPP

#include <vector>

namespace lynx
{
class pipeline
{
  public:
    pipeline(const char *vert_path, const char *frag_path);

  private:
    void init(const char *vert_path, const char *frag_path);
    static std::vector<char> read_file(const char *path);
};
} // namespace lynx

#endif