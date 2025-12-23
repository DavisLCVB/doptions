// Compile-time Builder Pattern Example
// Demuestra cómo implementar un Builder completamente en compile-time
// sin copias innecesarias, resultando en código eficiente y binarios pequeños

#include <iostream>
#include <string_view>
#include <type_traits>

// ============================================================================
// COMPILE-TIME BUILDER PATTERN
// ============================================================================

// Estructura final que será construida
template <typename NameType, typename PortType, typename TimeoutType>
struct ServerConfig {
  NameType name;
  PortType port;
  TimeoutType timeout;

  // Constructor constexpr - permite construcción en compile time
  constexpr ServerConfig(NameType n, PortType p, TimeoutType t)
      : name(n), port(p), timeout(t) {}

  // Método para mostrar la configuración
  constexpr void display() const {
    // En C++20 podemos usar std::cout en contextos constexpr limitados
    // pero para runtime lo hacemos normal
    std::cout << "Server Configuration:\n"
              << "  Name: " << name << "\n"
              << "  Port: " << port << "\n"
              << "  Timeout: " << timeout << " ms\n";
  }
};

// ============================================================================
// BUILDER CON TIPOS PHANTOM PARA SEGURIDAD EN COMPILE-TIME
// ============================================================================

// Estados del builder (Phantom Types)
struct Unset {};
struct Set {};

// Builder template con tipos phantom que rastrean qué se ha configurado
template <typename NameState = Unset, typename PortState = Unset,
          typename TimeoutState = Unset>
class ServerConfigBuilder {
 private:
  const char* name_ = "";
  int port_ = 0;
  int timeout_ = 0;

 public:
  // Constructor constexpr
  constexpr ServerConfigBuilder() = default;

  // Constructor privado para construcción interna
  constexpr ServerConfigBuilder(const char* n, int p, int t)
      : name_(n), port_(p), timeout_(t) {}

  // Setters que retornan un nuevo builder con el estado actualizado
  // Nota: Retornamos por valor, pero el compilador eliminará las copias (RVO)

  [[nodiscard]] constexpr auto withName(const char* name) const {
    return ServerConfigBuilder<Set, PortState, TimeoutState>(name, port_,
                                                               timeout_);
  }

  [[nodiscard]] constexpr auto withPort(int port) const {
    return ServerConfigBuilder<NameState, Set, TimeoutState>(name_, port,
                                                              timeout_);
  }

  [[nodiscard]] constexpr auto withTimeout(int timeout) const {
    return ServerConfigBuilder<NameState, PortState, Set>(name_, port_,
                                                           timeout);
  }

  // Build solo está disponible cuando todos los campos están configurados
  // Esto se verifica en COMPILE TIME usando enable_if
  template <typename N = NameState, typename P = PortState,
            typename T = TimeoutState>
  [[nodiscard]] constexpr auto build() const
      -> std::enable_if_t<std::is_same_v<N, Set> && std::is_same_v<P, Set> &&
                              std::is_same_v<T, Set>,
                          ServerConfig<const char*, int, int>> {
    return ServerConfig<const char*, int, int>(name_, port_, timeout_);
  }
};

// ============================================================================
// BUILDER ALTERNATIVO: MÁS SIMPLE, SIN PHANTOM TYPES
// ============================================================================

// Builder simple con configuración completa en compile-time
class SimpleConfigBuilder {
 private:
  const char* server_name_;
  int server_port_;
  int connection_timeout_;

 public:
  // Constructor constexpr
  constexpr SimpleConfigBuilder()
      : server_name_("localhost"), server_port_(8080),
        connection_timeout_(3000) {}

  // Setters fluidos que retornan referencia a sí mismo
  // Usamos [[nodiscard]] para evitar que se ignore el retorno
  [[nodiscard]] constexpr SimpleConfigBuilder&& name(const char* n) && {
    server_name_ = n;
    return std::move(*this);
  }

  [[nodiscard]] constexpr SimpleConfigBuilder&& port(int p) && {
    server_port_ = p;
    return std::move(*this);
  }

  [[nodiscard]] constexpr SimpleConfigBuilder&& timeout(int t) && {
    connection_timeout_ = t;
    return std::move(*this);
  }

  // Build method
  [[nodiscard]] constexpr auto build() && {
    return ServerConfig<const char*, int, int>(
        server_name_, server_port_, connection_timeout_);
  }
};

// ============================================================================
// EJEMPLO 3: BUILDER CON PARÁMETROS POR DEFECTO
// ============================================================================

// Versión más avanzada con valores por defecto en compile-time
template <int DefaultPort = 8080, int DefaultTimeout = 5000>
class AdvancedConfigBuilder {
 private:
  const char* name_;
  int port_;
  int timeout_;

 public:
  // Constructor con valores por defecto (evaluados en compile-time)
  constexpr AdvancedConfigBuilder()
      : name_("default-server"), port_(DefaultPort),
        timeout_(DefaultTimeout) {}

  constexpr AdvancedConfigBuilder&& withName(const char* n) && {
    name_ = n;
    return std::move(*this);
  }

  constexpr AdvancedConfigBuilder&& withPort(int p) && {
    port_ = p;
    return std::move(*this);
  }

  constexpr AdvancedConfigBuilder&& withTimeout(int t) && {
    timeout_ = t;
    return std::move(*this);
  }

  [[nodiscard]] constexpr auto build() && {
    return ServerConfig<const char*, int, int>(name_, port_, timeout_);
  }
};

// ============================================================================
// FUNCIÓN HELPER PARA CREAR BUILDER (C++17 CTAD)
// ============================================================================

// Helper function para deducción de tipos
constexpr auto createServerConfig() { return ServerConfigBuilder<>(); }

constexpr auto createSimpleConfig() { return SimpleConfigBuilder(); }

template <int Port = 8080, int Timeout = 5000>
constexpr auto createAdvancedConfig() {
  return AdvancedConfigBuilder<Port, Timeout>();
}

// ============================================================================
// FUNCIÓN MAIN - DEMOSTRACIÓN
// ============================================================================

auto main() -> int {
  std::cout << "=== COMPILE-TIME BUILDER PATTERN EXAMPLES ===\n\n";

  // EJEMPLO 1: Builder con Phantom Types (seguridad en compile-time)
  std::cout << "1. Builder con Phantom Types:\n";

  // Construcción fluida - todo verificado en compile time
  constexpr auto config1 = createServerConfig()
                               .withName("production-server")
                               .withPort(443)
                               .withTimeout(10000)
                               .build();

  config1.display();

  // DESCOMENTAR ESTO CAUSARÁ ERROR DE COMPILACIÓN:
  // auto invalid = createServerConfig().withName("test").build();
  // Error: build() requiere que todos los campos estén configurados

  std::cout << "\n2. Simple Builder (move semantics):\n";

  // EJEMPLO 2: Builder simple con move semantics
  // Nota: Usamos std::move() para hacer explícito el movimiento
  constexpr auto config2 = SimpleConfigBuilder()
                               .name("staging-server")
                               .port(3000)
                               .timeout(5000)
                               .build();

  config2.display();

  std::cout << "\n3. Advanced Builder con parámetros por defecto:\n";

  // EJEMPLO 3: Builder con valores por defecto en compile-time
  constexpr auto config3 = createAdvancedConfig<9000, 15000>()
                               .withName("custom-server")
                               .build();

  config3.display();

  // EJEMPLO 4: Configuración completamente en compile-time
  std::cout << "\n4. Configuración completamente compile-time:\n";

  constexpr auto config4 = []() {
    return AdvancedConfigBuilder<7777, 2000>()
        .withName("compile-time-server")
        .withPort(9999)
        .build();
  }();

  config4.display();

  // VERIFICACIÓN: Estas configuraciones se evalúan en compile-time
  std::cout << "\n=== VERIFICACIÓN DE COMPILE-TIME ===\n";
  std::cout << "Todas las configuraciones son constexpr: "
            << "Sí (verificado por el compilador)\n";

  std::cout << "\nVENTAJAS DE ESTE ENFOQUE:\n"
            << "  ✓ Sin copias innecesarias (RVO + move semantics)\n"
            << "  ✓ Verificación de tipos en compile-time\n"
            << "  ✓ Código generado óptimo (todo inline)\n"
            << "  ✓ Tamaño de binario mínimo\n"
            << "  ✓ Zero-cost abstraction\n";

  return 0;
}

/*
 * COMPILACIÓN Y EJECUCIÓN:
 *
 * g++ -std=c++17 -O2 compile-time-builder-example.cpp -o builder
 * ./builder
 *
 * PARA VERIFICAR QUE NO HAY COPIAS:
 * g++ -std=c++17 -O2 -S compile-time-builder-example.cpp
 * # Revisar el assembly generado - verás que no hay llamadas a constructores
 * de copia
 *
 * PARA VERIFICAR TAMAÑO DEL BINARIO:
 * g++ -std=c++17 -O2 compile-time-builder-example.cpp -o builder
 * strip builder
 * ls -lh builder
 * # El binario será muy pequeño debido a la optimización compile-time
 *
 * CONCEPTOS CLAVE DEMOSTRADOS:
 *
 * 1. CONSTEXPR: Todas las funciones y constructores son constexpr,
 *    permitiendo evaluación en compile-time cuando es posible.
 *
 * 2. PHANTOM TYPES: El primer builder usa tipos phantom (Unset/Set) para
 *    rastrear en compile-time qué campos han sido configurados. Esto previene
 *    errores en compile-time.
 *
 * 3. MOVE SEMANTICS: El builder retorna *this por rvalue reference (&&),
 *    permitiendo encadenamiento sin copias.
 *
 * 4. RVO (Return Value Optimization): El compilador elimina copias automáticamente
 *    cuando retornamos objetos por valor.
 *
 * 5. [[nodiscard]]: Previene que se ignore el valor retornado, forzando
 *    uso correcto del builder.
 *
 * 6. ENABLE_IF: Usado para habilitar build() solo cuando todos los campos
 *    están configurados (verificación en compile-time).
 *
 * 7. TEMPLATE PARAMETERS: Valores por defecto en templates se resuelven
 *    en compile-time, sin overhead en runtime.
 */
