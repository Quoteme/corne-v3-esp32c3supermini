class Keyboard {
public:
  Keyboard() = default;
  Keyboard(Keyboard &&) = default;
  Keyboard(const Keyboard &) = default;
  Keyboard &operator=(Keyboard &&) = default;
  Keyboard &operator=(const Keyboard &) = default;
  ~Keyboard() = default;

private:
};
