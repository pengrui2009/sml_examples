//
// Copyright (c) 2016-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/sml.hpp>
#include <cassert>
#include <iostream>

namespace sml = boost::sml;

namespace {

struct evt_enter_silentota {};
struct evt_enter_unsilentota {};
struct evt_enter_swdlota {};
struct evt_enter_usbota {};

struct evt_exit_silentota {};
struct evt_exit_unsilentota {};
struct evt_exit_swdlota {};
struct evt_exit_usbota {};

struct evt_silentota_download {};

struct e1 {};
struct e2 {};
struct e3 {};
struct e4 {};
struct e5 {};

class Manager {
 public:
  void set_state(const std::string &state) { state_ = state; }
  const std::string &get_state() const { return state_; }
  void guard() {
    std::cout << "guard00" << std::endl;
  }
  void action() {
    std::cout << "action00" << std::endl;
  }

  void guard1() {
    std::cout << "guard11" << std::endl;
  }
  void action1() {
    std::cout << "action11" << std::endl;
  }
 private:
  std::string state_;
};

auto guard10 = [] (Manager &m) {
    std::cout << "guard1" << std::endl;
    m.guard1();
    return true;
  };
  
  auto action10 = [] (Manager &m) {
    std::cout << "action1" << std::endl;
    m.action1();
    return true;
  };

auto guard11 = [] (Manager &m) {
  std::cout << "guard1" << std::endl;
  m.guard();
  return true;
};

auto action11 = [] (Manager &m) {
  std::cout << "action1" << std::endl;
  m.action();
  return true;
};

struct SilentOTA {
  using self = SilentOTA;

  auto operator()() const noexcept {
    using namespace sml;

    auto guard = [] {
      std::cout << "guard" << std::endl;
      return true;
    };

    auto action = [] {
      std::cout << "Enter in SilentOTA sm download state" << std::endl;
      return true;
    };
    auto action2 = [] {
      std::cout << "action2" << std::endl;
      return true;
    };
    // clang-format off
      return make_transition_table(
    //    *"idle"_s        + event<e3> [ guard ] / action = "download"_s
       *"download"_s    + event<e3> [ guard10 ] / action10 = "download_success"_s
       ,"download"_s    + event<e3> [ guard ] / [] { std::cout << "in SilentOTA sm" << std::endl; } = "download_failed"_s
       ,"download_success"_s    + event<e3> [ guard ] / [] { std::cout << "in SilentOTA sm" << std::endl; } = "install"_s
      , "install"_s     + event<e3> [ guard ] / [] { std::cout << "in SilentOTA sm" << std::endl; } = "switchslot"_s
      , "switchslot"_s  + event<e4> [ guard ] / [] { std::cout << "finish SilentOTA sm" << std::endl; } = X
      );
    // clang-format on
  }

  bool guard(int i) const noexcept {
    // assert(42 == i);
    std::cout << "guard SilentOTA download sm" << std::endl;
    return true;
  }
  void action(int i, const e3 &) {
    // assert(42 == i);
    std::cout << "action SilentOTA download sm" << std::endl;
  }
};

struct UnSilentOTA {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
        return make_transition_table(
         *"idle"_s + event<e3> / [] { std::cout << "in sub sm" << std::endl; } = "s1"_s
        , "s1"_s + event<e4> / [] { std::cout << "finish sub sm" << std::endl; } = X
        );
    // clang-format on
  }
};

struct SWDLOTA {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
        return make_transition_table(
         *"idle"_s + event<e3> / [] { std::cout << "in sub sm" << std::endl; } = "s1"_s
        , "s1"_s + event<e4> / [] { std::cout << "finish sub sm" << std::endl; } = X
        );
    // clang-format on
  }
};

struct USBOTA {
  using self = USBOTA;

  auto operator()() const noexcept {
    using namespace sml;

    auto guard = [] {
        std::cout << "guard" << std::endl;
        return true;
      };
  
      auto action = [] {
        std::cout << "Enter in SilentOTA sm download state" << std::endl;
        return true;
      };
    // clang-format off
        return make_transition_table(
         *"idle"_s + event<e3> / [] { std::cout << "in sub sm" << std::endl; } = "s1"_s
        , "s1"_s + event<e4> [ guard ] / action = X
        );
    // clang-format on
  }

    bool guard(int i) const noexcept {
        // assert(42 == i);
        std::cout << "guard USBOTA sm" << std::endl;
        return true;
    }

    void action(int i, const e3 &) {
        // assert(42 == i);
        std::cout << "action USBOTA sm" << std::endl;
    }
};

struct composite {
  using self = composite;

  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
     *"idle"_s              + event<e1> = "s1"_s
    , "s1"_s                + event<evt_enter_silentota>     [ guard11 ] / action11 = state<SilentOTA>
    , "s1"_s                + event<evt_enter_unsilentota>    / [] { std::cout << "enter UnSilentOTA sm" << std::endl; } = state<UnSilentOTA>
    , "s1"_s                + event<evt_enter_swdlota>        / [] { std::cout << "enter SWDLOTA sm" << std::endl; } = state<SWDLOTA>
    , "s1"_s                + event<evt_enter_usbota>         / [] { std::cout << "enter USBOTA sm" << std::endl; } = state<USBOTA>
    , state<SilentOTA>      + event<evt_exit_silentota>       / [] { std::cout << "exit SilentOTA sm" << std::endl; } = X
    , state<UnSilentOTA>    + event<evt_exit_unsilentota>     / [] { std::cout << "exit UnSilentOTA sm" << std::endl; } = X
    , state<SWDLOTA>        + event<evt_exit_swdlota>         / [] { std::cout << "exit SWDLOTA sm" << std::endl; } = X
    , state<USBOTA>         + event<evt_exit_usbota>          / [] { std::cout << "exit USBOTA sm" << std::endl; } = X
    );
    // clang-format on
  }

  bool guard(int i) const noexcept {
    // assert(42 == i);
    std::cout << "guard SilentOTA sm" << std::endl;
    return true;
  }

  void action(int i, const evt_enter_silentota &) {
    // assert(42 == i);
    std::cout << "action SilentOTA sm" << std::endl;
  }
};
}  // namespace

int main(int argc, char *argv[]) {
  Manager m{};
//   SilentOTA silentota{};
//   USBOTA usbota{};
//   composite c{};
  sml::sm<composite, SilentOTA, USBOTA> sm{m};

  using namespace sml;
  //   assert(sm.is("idle"_s));
  //   assert(sm.is<decltype(state<sub>)>("idle"_s));

  sm.process_event(e1{});
  assert(sm.is("s1"_s));
  //   assert(sm.is<decltype(state<sub>)>("idle"_s));

  sm.process_event(evt_enter_silentota{});  // enter sub sm
  assert(sm.is(state<SilentOTA>));
  assert(sm.is<decltype(state<SilentOTA>)>("download"_s));
  //   sm.visit_current_states(state_name_visitor<decltype(sm)>{sm});
  sm.visit_current_states([](auto state) { std::cout << state.c_str() << std::endl; });

  sm.process_event(e3{});  // in sub sm
  assert(sm.is(state<SilentOTA>));
  assert(sm.is<decltype(state<SilentOTA>)>("download_success"_s));

  //   sm.process_event(e4{});  // finish sub sm
  //   assert(sm.is(state<sub>));
  //   assert(sm.is<decltype(state<sub>)>(X));

  sm.process_event(evt_exit_usbota{});  // exit sub sm
  //   assert(sm.is(X));
  //   assert(sm.is<decltype(state<sub>)>(X));
}
