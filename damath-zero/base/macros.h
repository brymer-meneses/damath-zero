#define REQUIRE_CONCEPT(concept, object)                               \
  static_assert(concept<object>,                                       \
                "The " #object " does not conform to the interface " # \
                concept ".")
