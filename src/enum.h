#ifndef enum_h
#define enum_h

// Enum para definir os tipos de etapas no processo de produção
enum StepType
{
    none,          // Sem etapa definida
    mash,          // Etapa de maceração
    boil,          // Etapa de fervura
    anticavitation // Etapa de anticavitação
};

// Estrutura para armazenar as temperaturas em diferentes etapas
struct Temperatures
{
    float Main;      // Temperatura principal
    float Sparge;    // Temperatura de sparge
    float Boil;      // Temperatura de fervura
    float AuxOne;    // Temperatura auxiliar 1
    float AuxTwo;    // Temperatura auxiliar 2
    float AuxThree;  // Temperatura auxiliar 3
};

#endif
