# Patrón Builder en Compile-Time C++

Este ejemplo demuestra cómo implementar el patrón Builder completamente en **compile-time** usando C++ moderno, evitando copias innecesarias y manteniendo el ejecutable pequeño.

## 🎯 Objetivos del Ejemplo

1. **Compile-time evaluation**: Todo se evalúa en tiempo de compilación cuando es posible
2. **Zero copias innecesarias**: Uso de move semantics y RVO (Return Value Optimization)
3. **Type-safety**: Verificación de tipos en compile-time usando Phantom Types
4. **Binario pequeño**: Código optimizado sin overhead en runtime

## 📚 Tres Implementaciones del Builder

### 1. Builder con Phantom Types (Type-Safe)

```cpp
constexpr auto config = createServerConfig()
                           .withName("production-server")
                           .withPort(443)
                           .withTimeout(10000)
                           .build();
```

**Ventajas:**
- ✅ Verificación en compile-time de campos requeridos
- ✅ Error de compilación si faltan campos
- ✅ Seguridad total en tipos

**Características técnicas:**
- Usa tipos phantom (`Unset`/`Set`) para rastrear estado
- `build()` solo disponible cuando todos los campos están configurados
- `enable_if` para habilitar/deshabilitar métodos

### 2. Simple Builder (Move Semantics)

```cpp
constexpr auto config = SimpleConfigBuilder()
                           .name("staging-server")
                           .port(3000)
                           .timeout(5000)
                           .build();
```

**Ventajas:**
- ✅ Sintaxis más simple
- ✅ Move semantics explícito (`&&`)
- ✅ Sin copias gracias a rvalue references

**Características técnicas:**
- Retorna `*this` por rvalue reference
- Encadenamiento fluido sin copias
- Valores por defecto

### 3. Advanced Builder (Template Parameters)

```cpp
constexpr auto config = createAdvancedConfig<9000, 15000>()
                           .withName("custom-server")
                           .build();
```

**Ventajas:**
- ✅ Valores por defecto en compile-time
- ✅ Template parameters para configuración
- ✅ Máxima flexibilidad

**Características técnicas:**
- Parámetros template con valores por defecto
- Todo resuelto en compile-time
- Zero overhead en runtime

## 🔧 Compilación y Uso

### Compilar el ejemplo:

```bash
# Con CMake
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/compile-time-builder-example

# Directamente con g++
g++ -std=c++17 -O2 compile-time-builder-example.cpp -o builder
./builder
```

### Verificar que no hay copias (análisis assembly):

```bash
g++ -std=c++17 -O2 -S compile-time-builder-example.cpp
# Revisar compile-time-builder-example.s
# No verás llamadas a constructores de copia
```

### Verificar tamaño del binario:

```bash
g++ -std=c++17 -O2 compile-time-builder-example.cpp -o builder
strip builder
ls -lh builder  # ~15KB - muy pequeño!
```

## 🧠 Conceptos Clave Explicados

### 1. `constexpr` - Compile-Time Evaluation

```cpp
constexpr ServerConfigBuilder() = default;
```

Permite que el código se evalúe en tiempo de compilación, resultando en:
- **Mejor rendimiento**: Sin cálculos en runtime
- **Binarios más pequeños**: Valores precalculados
- **Optimizaciones**: El compilador puede inline todo

### 2. Phantom Types - Type-Safety

```cpp
template <typename NameState = Unset, typename PortState = Unset>
class ServerConfigBuilder { /* ... */ };
```

Los tipos phantom rastrean el estado **sin ocupar memoria**:
- `Unset`: Campo no configurado
- `Set`: Campo configurado
- El compilador verifica en compile-time

### 3. Move Semantics - Zero Copias

```cpp
[[nodiscard]] constexpr SimpleConfigBuilder&& name(const char* n) && {
    server_name_ = n;
    return std::move(*this);
}
```

Retornar rvalue reference (`&&`) permite:
- **Encadenamiento**: Llamadas fluidas
- **Sin copias**: Move en lugar de copy
- **Eficiencia**: Compilador optimiza

### 4. `[[nodiscard]]` - Correctness

```cpp
[[nodiscard]] constexpr auto build() && { /* ... */ }
```

Previene errores comunes:
- Fuerza uso del valor retornado
- Warning si se ignora el resultado
- Mejora la correctitud del código

### 5. `enable_if` - SFINAE

```cpp
template <typename N = NameState>
constexpr auto build() const
    -> std::enable_if_t<std::is_same_v<N, Set>, ServerConfig> {
    // Solo disponible si N == Set
}
```

Habilita métodos condicionalmente:
- Verificación en compile-time
- Mensajes de error claros
- Type-safety garantizado

### 6. RVO (Return Value Optimization)

El compilador elimina automáticamente copias cuando se retorna por valor:

```cpp
constexpr auto build() && {
    return ServerConfig(name_, port_, timeout_);
    // NO HAY COPIA - el compilador construye directamente en destino
}
```

## 📊 Comparación de Performance

| Método | Copias | Compile-time | Type-safe | Tamaño binario |
|--------|--------|--------------|-----------|----------------|
| Builder tradicional | 3-4 | ❌ No | ⚠️ Parcial | Grande |
| **Este ejemplo** | **0** | **✅ Sí** | **✅ Total** | **Mínimo** |

## 🎓 Cuándo Usar Cada Builder

### Phantom Types Builder
- ✅ APIs públicas donde la correctitud es crítica
- ✅ Cuando necesitas garantías en compile-time
- ✅ Configuraciones complejas con muchos campos

### Simple Builder
- ✅ Uso interno en tu código
- ✅ Configuraciones simples
- ✅ Cuando prefieres simplicidad sobre type-safety extremo

### Advanced Builder
- ✅ Necesitas valores por defecto configurables
- ✅ Múltiples variantes de la misma clase
- ✅ Máxima flexibilidad en compile-time

## 🚀 Ventajas de este Enfoque

1. **Zero-Cost Abstraction**: No hay overhead en runtime
2. **Type-Safety**: Errores detectados en compile-time
3. **Optimización**: Compilador puede inline todo
4. **Mantenibilidad**: Código expresivo y seguro
5. **Portabilidad**: C++17/20 estándar

## 📖 Lecturas Recomendadas

- [C++ constexpr](https://en.cppreference.com/w/cpp/language/constexpr)
- [Move Semantics](https://en.cppreference.com/w/cpp/language/move_constructor)
- [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae)
- [Phantom Types](https://www.foonathan.net/2016/10/strong-typedefs/)
- [Builder Pattern](https://refactoring.guru/design-patterns/builder)

## ⚡ Tips de Optimización

1. **Usa `constexpr` siempre que sea posible**
   ```cpp
   constexpr auto config = createConfig().build();
   // Se evalúa en compile-time si los parámetros son constantes
   ```

2. **Retorna por rvalue reference para encadenamiento**
   ```cpp
   Builder&& method() && { return std::move(*this); }
   ```

3. **Marca con `[[nodiscard]]` métodos builder**
   ```cpp
   [[nodiscard]] auto build() const { /* ... */ }
   ```

4. **Usa template parameters para valores por defecto**
   ```cpp
   template <int DefaultValue = 42>
   class Builder { /* ... */ };
   ```

## 🐛 Errores Comunes y Soluciones

### Error: No se puede llamar `build()` sin configurar todos los campos

```cpp
// ❌ Error
auto config = createServerConfig()
                 .withName("server")
                 .build();  // Error: falta port y timeout

// ✅ Correcto
auto config = createServerConfig()
                 .withName("server")
                 .withPort(8080)
                 .withTimeout(5000)
                 .build();
```

### Error: Uso del builder después de `build()`

```cpp
// ❌ Error
auto builder = SimpleConfigBuilder().name("test");
auto config = std::move(builder).build();
builder.port(8080);  // Error: builder fue movido

// ✅ Correcto
auto config = SimpleConfigBuilder()
                 .name("test")
                 .port(8080)
                 .build();
```

## 📝 Licencia

Este ejemplo es parte del proyecto DOptions y está bajo la misma licencia.
