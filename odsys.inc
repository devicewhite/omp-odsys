// ==============================================
// Project: Optmized Distance System (ODSYS)
// Date: 02/SET/2024 14:04:50 BRT
// Author: DeviceBlack
// ==============================================

#if defined _INC_a_samp
	#error Biblioteca suportada apenas no Open.MP!
#endif

#include <open.mp> // Inclui a biblioteca principal do Open Multiplayer (OpenMP)

#if !defined ODSYS_FRAMERATE
	#define ODSYS_FRAMERATE	(250)
#endif

static enum METER_INFO
{
	Float:f_posx,  // Posição X do veículo
	Float:f_posy,  // Posição Y do veículo
	Float:f_posz,  // Posição Z do veículo
	Float:f_meter  // Distância percorrida em metros
};

static
	i_timer,			// ID do timer
	i_vehicles,		  // Número de veículos ativos atualmente
	a_vehicle[MAX_VEHICLES], // Array que armazena os IDs dos veículos ativos
	a_info[MAX_VEHICLES][METER_INFO]; // Array para armazenar informações de posição e distância para cada veículo

// ==============================================
// Função: OnVehicleSpawn
// Descrição: Callback que é executado quando um veículo é spawnado no servidor.
// Parâmetro:
//   - vehicleid: ID do veículo que foi spawnado.
// Retorno: 
//   - Retorna 1 para indicar que o veículo foi spawnado corretamente, 
//	 ou chama outra função se definida por outro script.
// ==============================================
public OnVehicleSpawn(vehicleid)
{
	// Atualiza a lista de veículos ativos e o número de veículos ativos.
	i_vehicles = GetVehicles(a_vehicle);

	// Inicializa a distância percorrida pelo veículo como 0 e armazena a posição inicial.
	a_info[vehicleid][f_meter] = 0.0;
	GetVehiclePos(vehicleid, a_info[vehicleid][f_posx], a_info[vehicleid][f_posy], a_info[vehicleid][f_posz]);

	// Inicia o timer se ainda não estiver ativo e este for o primeiro veículo.
	if(i_vehicles == 1 && !i_timer)
		i_timer = SetTimer("UpdateVehicleMeter", ODSYS_FRAMERATE, true);

	// Verifica se há uma função definida para ODSYS_VehicleSpawn. Se sim, chama-a.
	#if defined ODSYS_VehicleSpawn
		return ODSYS_VehicleSpawn(vehicleid);
	#else
		return 1; // Caso contrário, retorna 1 indicando que o veículo foi spawnado com sucesso.
	#endif
}

// Manipulação para garantir compatibilidade com outros scripts que usam OnVehicleSpawn.
#if defined ALS_OnVehicleSpawn
	#undef OnVehicleSpawn
#else
	#define ALS_OnVehicleSpawn
#endif
#define OnVehicleSpawn ODSYS_VehicleSpawn
#if defined ODSYS_VehicleSpawn
	forward ODSYS_VehicleSpawn(vehicleid);
#endif

// ==============================================
// Forward: UpdateVehicleMeter
// Descrição: Função forward para declarar a função UpdateVehicleMeter antes de sua implementação.
// ==============================================
forward UpdateVehicleMeter();

// ==============================================
// Função: UpdateVehicleMeter
// Descrição: Atualiza as distâncias percorridas por todos os veículos ativos.
// Retorno: 
//   - Nenhum retorno. Realiza atualizações internas das posições e distâncias.
// ==============================================
public UpdateVehicleMeter()
{
	new bool:resync = false; // Inicializa a variável resync como falsa

	// Loop para percorrer todos os veículos ativos e atualizar suas distância
	for(new i, v; i < i_vehicles; i++)
	{
		v = a_vehicle[i]; // Obtém o ID do veículo

		// Verifica se o veículo ainda é válido
		if(IsValidVehicle(v))
		{
			// Atualiza a distância e a posição do veículo
			a_info[v][f_meter] += GetVehicleDistanceFromPoint(v, a_info[v][f_posx], a_info[v][f_posy], a_info[v][f_posz]);
			GetVehiclePos(v, a_info[v][f_posx], a_info[v][f_posy], a_info[v][f_posz]);
		}
		else resync = true; // Marca que uma resincronização é necessária se o veículo for inválido
	}

	// Se for necessário resincronizar (ex.: veículos removidos)
	if(resync)
	{
		i_vehicles = GetVehicles(a_vehicle); // Atualiza a lista de veículos ativos
		
		// Se não houver mais veículos ativos, mata o timer para evitar execuções desnecessárias
		if(i_vehicles == 0 && i_timer)
		{
			KillTimer(i_timer);
			i_timer = 0; // Reseta o ID do timer para indicar que não está mais ativo
		}
	}
}

// ==============================================
// Função: GetVehicleMeters
// Descrição: Retorna a distância percorrida por um veículo em metros.
// Parâmetro:
//   - vehicleid: ID do veículo do qual deseja obter a distância percorrida.
// Retorno: 
//   - Distância percorrida em metros como um valor float.
// ==============================================
stock Float:GetVehicleMeters(vehicleid)
{
	return a_info[vehicleid][f_meter];
}

// ==============================================
// Função: GetVehicleKilometers
// Descrição: Retorna a distância percorrida por um veículo em quilômetros.
// Parâmetro:
//   - vehicleid: ID do veículo do qual deseja obter a distância percorrida.
// Retorno: 
//   - Distância percorrida em quilômetros como um valor float.
// ==============================================
stock Float:GetVehicleKilometers(vehicleid)
{
	return (a_info[vehicleid][f_meter] * 0.001); // Converte metros para quilômetros
}

// ==============================================
// Função: GetVehicleMiles
// Descrição: Retorna a distância percorrida por um veículo em milhas.
// Parâmetro:
//   - vehicleid: ID do veículo do qual deseja obter a distância percorrida.
// Retorno: 
//   - Distância percorrida em milhas como um valor float.
// ==============================================
stock Float:GetVehicleMiles(vehicleid)
{
	return (a_info[vehicleid][f_meter] * 0.000621); // Converte metros para milhas
}

// ==============================================
// Função: ResetVehicleMeters
// Descrição: Reseta a distância percorrida por um veículo para 0.
// Parâmetro:
//   - vehicleid: ID do veículo cuja distância deseja resetar.
// Retorno: 
//   - Retorna true se o veículo for válido e a distância for resetada com sucesso.
//   - Retorna false se o veículo for inválido.
// ==============================================
stock bool:ResetVehicleMeters(vehicleid)
{
	if(IsValidVehicle(vehicleid)) // Verifica se o veículo é válido
	{
		a_info[vehicleid][f_meter] = 0.0; // Reseta a distância percorrida
		return true; // Sucesso na operação
	}
	return false; // Falha na operação (veículo inválido)
}