#include <boost/sml.hpp>
#include <cassert>
#include <iostream>
namespace sml = boost::sml;


namespace {

  struct e1 {};
  struct e2 {};
  struct e3 {};
  struct e4 {};
  struct e5 {};
  
  // Allow out of header implementation
  // bool guard2_impl(int);
  
  struct transitions {
    using self = transitions;
  
    auto operator()() {
      using namespace sml;
  
      auto guard1 = [] {
        std::cout << "guard1" << std::endl;
        return true;
      };
  
      auto guard2 = [] {

      };
  
      auto action1 = [](auto e) { std::cout << "action1: " << typeid(e).name() << std::endl; };
  
      struct action2 {
        void operator()([[maybe_unused]] int i) {
          assert(42 == i);
          std::cout << "action2" << std::endl;
        }
      };
  
      // clang-format off
      return make_transition_table(
          *"idle"_s + event<e1> = "s1"_s
        , "s1"_s + event<e2> [ guard1 ] / action1 = "s2"_s
        , "s2"_s + event<e3> [ guard1 && ![] { return false;} ] / (action1, action2{}) = "s3"_s
        , "s3"_s + event<e4> [ !guard1 || guard2 ] / (action1, [] { std::cout << "action3" << std::endl; }) = "s4"_s
        , "s3"_s + event<e4> [ guard1 ] / ([] { std::cout << "action4" << std::endl; }, [this] { action4(); } ) = "s5"_s
        , "s5"_s + event<e5> [ &self::guard3 ] / &self::action5 = X
      );
      // clang-format on
    }
  
    bool guard3(int i) const noexcept {
      assert(42 == i);
      std::cout << "guard3" << std::endl;
      return true;
    }
  
    void action4() const { std::cout << "action4" << std::endl; }
  
    void action5(int i, const e5&) {
      assert(42 == i);
      std::cout << "action5" << std::endl;
    }
  };
}

// namespace {
// struct transitions {
//   using self = transitions;

//   auto operator()() const noexcept {
//     using namespace sml;

//     auto guard1 = [] {
//       std::cout << "guard1" << std::endl;
//       return true;
//     };

//     auto action30 = [] {
//       std::cout << "action3" << std::endl;
//       return true;
//     };

//     return make_transition_table(
//        *"idle"_s                    / [] { std::cout << "anonymous transition" << std::endl; } = "s1"_s
//       , "s1"_s + event<e1> [guard1] / [this] { }
//       , "s1"_s + event<e2>          / [] { std::cout << "self transition" << std::endl; } = "s1"_s
//       , "s1"_s + sml::on_entry<_>   / [] { std::cout << "s1 entry" << std::endl; }
//       , "s1"_s + sml::on_exit<_>    / [] { std::cout << "s1 exit" << std::endl; }
//       // , "s1"_s + event<e3> [ &self::guard3] / &self::action3 
//       , "s5"_s + event<e5> [ &self::guard11 ] / action30 = X
//     );
//   }

//   bool guard11(int i) const noexcept {
//     std::cout << "guard1" << std::endl;
//     return true;
//   }

//   void action1(int i, const e5&) {  
//     std::cout << "action1" << std::endl;
//     // return true;
//   }

//   bool guard3(int i) const noexcept {
//     assert(42 == i);
//     std::cout << "guard3" << std::endl;
//     return true;
//   }

//   void action3(int i, const e3&) {  
//     assert(42 == i);
//     std::cout << "action1" << std::endl;
//     // return true;
//   }

//   bool guard5(int i) const noexcept {
//     assert(42 == i);
//     std::cout << "guard5" << std::endl;
//     return true;
//   }

//   void action5(int i, const e5&) {
//     assert(42 == i);
//     std::cout << "action5" << std::endl;
//   }
// };
// }  // namespace

int main() {
  transitions t{};
  sml::sm<transitions> sm{t};
  // sm.visit_current_states();
  sm.process_event(e1{});
  //anonymous transition
  //s1 entry
  // sm.process_event(e2{});
  //internal transition
  // sm.process_event(e3{});
  //s1 exit
  //self transition
  //s1 entry
  // assert(sm.is(sml::X));
  //s1 exit
  //external transition
  return 0;
}