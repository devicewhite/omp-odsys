# **ODSYS - Optimized Distance System**

**ODSYS** é um sistema altamente otimizado para monitorar a distância percorrida por veículos em servidores Open Multiplayer (utilizando a biblioteca **open.mp**). O sistema permite calcular com precisão a distância em metros, quilômetros e milhas, além de fornecer funcionalidades para resetar a quilometragem dos veículos.

## **Características**

- **Medição Precisa de Distância:** Monitore a distância percorrida por veículos em tempo real, com suporte para metros, quilômetros e milhas.
- **Alta Performance:** Código otimizado para minimizar o impacto no desempenho do servidor, mesmo com um grande número de veículos.
- **Fácil Integração:** Simples de integrar em qualquer gamemode, com suporte a callbacks padrões do Open.MP.

## **Como Funciona**

O sistema utiliza callbacks nativos e timers para rastrear a posição dos veículos em intervalos regulares, calculando a distância percorrida desde o último ponto de verificação. Os dados de distância são armazenados em arrays específicos para cada veículo, permitindo um rastreamento contínuo e eficiente.

### **Principais Funções**

- `Float:GetVehicleMeters(vehicleid)`: Retorna a distância percorrida pelo veículo em metros.
- `Float:GetVehicleKilometers(vehicleid)`: Retorna a distância percorrida pelo veículo em quilômetros.
- `Float:GetVehicleMiles(vehicleid)`: Retorna a distância percorrida pelo veículo em milhas.
- `bool:ResetVehicleDistance(vehicleid)`: Reseta a distância percorrida pelo veículo para `0.0`.
- `bool:SetVehicleDistance(vehicleid, Float:distance)`: Define a distância percorrida pelo veículo.
- `bool:IsVehicleBeingTracked(vehicleid)`: Verifica se um veículo está sendo rastreado pelo sistema.
- `bool:DisableVehicleTracking(vehicleid)`: Desabilita o rastreamento de um veículo.
- `bool:EnableVehicleTracking(vehicleid)`: Habilita o rastreamento de um veículo.

## **Exemplo de Uso**

```pawn
public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
    ResetVehicleMeters(vehicleid);
    return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
    new Float:km = GetVehicleKilometers(vehicleid);
    SendClientMessage(playerid, -1, "Distancia percorrida: %.1f kilometros", km);
    return 1;
}
```

Este exemplo mostra como você pode utilizar o sistema ODSYS para monitorar a distância percorrida por veículos.

## **Instalação**

1. Clone ou baixe o repositório.
2. Inclua o arquivo `odsys.inc` no seu projeto Open.MP.
3. Chame as funções de `odsys` conforme necessário no seu script.

```pawn
#include <odsys>
```
4. Compile o seu script com o novo sistema integrado.

## **Contribuição**

Contribuições são bem-vindas! Sinta-se à vontade para abrir uma issue ou enviar um pull request.

## **Licença**

Este projeto é licenciado sob a MIT License - veja o arquivo [LICENSE](LICENSE) para mais detalhes.
