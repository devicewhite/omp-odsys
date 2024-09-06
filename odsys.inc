// ==============================================
// Project: Optimized Distance System (ODSYS)
// Date: 05/SET/2024 23:55:19 BRT
// Author: DeviceBlack
// ==============================================

#if defined _INC_a_samp
	#error Esta biblioteca é suportada apenas no Open.MP!
#endif

#include <open.mp> // Inclui a biblioteca principal do Open Multiplayer (Open.MP)

#if !defined ODSYS_FRAMERATE
	#define ODSYS_FRAMERATE (250) // Taxa de atualização do sistema em milissegundos
#endif

#if !defined ODSYS_DEFAULT_MODE
	#define ODSYS_DEFAULT_MODE true // Define se o modo padrão está ativado
#endif

#if !defined ODSYS_MILES
	#define ODSYS_MILES (0.000621) // Fator de conversão de metros para milhas
#endif

#if !defined ODSYS_KILOMETERS
	#define ODSYS_KILOMETERS (0.001) // Fator de conversão de metros para quilômetros
#endif

// Enum para armazenar informações do veículo
static enum VEHICLE_INFO
{
	Float:f_posx,    // Posição X do veículo
	Float:f_posy,    // Posição Y do veículo
	Float:f_posz,    // Posição Z do veículo
	Float:f_distance // Distância percorrida em metros
};

// Variáveis globais
static
	i_timer,                             // ID do timer
	i_vehicles,                          // Número de veículos ativos no momento
	a_vehicle[MAX_VEHICLES],             // Array para armazenar IDs dos veículos ativos
	a_info[MAX_VEHICLES][VEHICLE_INFO];  // Array para armazenar posição e distância de cada veículo

// ==============================================
// Função: OnVehicleSpawn
// Descrição: Callback executada quando um veículo é spawnado no servidor.
// ==============================================
public OnVehicleSpawn(vehicleid)
{
	if(ODSYS_DEFAULT_MODE)
	{
		// Atualiza a lista de veículos ativos e o número de veículos.
		i_vehicles = GetVehicles(a_vehicle);
	}

	// Inicializa a distância percorrida pelo veículo e armazena a posição inicial.
	if(IsVehicleBeingTracked(vehicleid))
	{
		a_info[vehicleid][f_distance] = 0.0;
		GetVehiclePos(vehicleid, a_info[vehicleid][f_posx], a_info[vehicleid][f_posy], a_info[vehicleid][f_posz]);

		// Inicia o timer se for o primeiro veículo e o timer ainda não estiver ativo.
		if(i_vehicles == 1 && !i_timer)
			i_timer = SetTimer("UpdateVehicleDistance", ODSYS_FRAMERATE, true);
	}

	// Verifica se ODSYS_VehicleSpawn está definido. Se estiver, chama essa função.
	#if defined ODSYS_VehicleSpawn
		return ODSYS_VehicleSpawn(vehicleid);
	#else
		return 1; // Retorna 1 indicando que o veículo foi spawnado com sucesso.
	#endif
}

// Compatibilidade com outros scripts que utilizam OnVehicleSpawn.
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
// Forward: UpdateVehicleDistance
// Descrição: Declaração da função UpdateVehicleDistance antes de sua implementação.
// ==============================================
forward UpdateVehicleDistance();

// ==============================================
// Função: UpdateVehicleDistance
// Descrição: Atualiza a distância percorrida por todos os veículos ativos.
// ==============================================
public UpdateVehicleDistance()
{
	if(i_vehicles)
	{
		// Loop para atualizar a distância de todos os veículos ativos
		for(new i, v; i < i_vehicles; i++)
		{
			v = a_vehicle[i]; // ID do veículo

			// Verifica se o veículo ainda é válido
			if(IsValidVehicle(v))
			{
				// Atualiza a distância e posição do veículo
				a_info[v][f_distance] += GetVehicleDistanceFromPoint(v, a_info[v][f_posx], a_info[v][f_posy], a_info[v][f_posz]);
				GetVehiclePos(v, a_info[v][f_posx], a_info[v][f_posy], a_info[v][f_posz]);
			}
			else
			{
				// Remove o veículo inválido da lista e atualiza os índices restantes
				for(new j = i; j < i_vehicles; j++) a_vehicle[j] = a_vehicle[j + 1];
				i_vehicles--;
			}
		}
	}
	else
	{
		// Se não houver veículos ativos, mata o timer e reseta o ID do timer
		i_timer = KillTimer(i_timer);
	}
}

// ==============================================
// Função: GetVehicleDistanceMeters
// Descrição: Retorna a distância percorrida por um veículo em metros.
// ==============================================
stock Float:GetVehicleDistanceMeters(vehicleid)
{
	return a_info[vehicleid][f_distance]; // Obtem a distancia em metros
}

// ==============================================
// Função: GetVehicleDistanceKilometers
// Descrição: Retorna a distância percorrida por um veículo em quilômetros.
// ==============================================
stock Float:GetVehicleDistanceKilometers(vehicleid)
{
	return (a_info[vehicleid][f_distance] * ODSYS_KILOMETERS); // Converte metros para quilômetros
}

// ==============================================
// Função: GetVehicleDistanceMiles
// Descrição: Retorna a distância percorrida por um veículo em milhas.
// ==============================================
stock Float:GetVehicleDistanceMiles(vehicleid)
{
	return (a_info[vehicleid][f_distance] * ODSYS_MILES); // Converte metros para milhas
}

// ==============================================
// Função: ResetVehicleDistance
// Descrição: Reseta a distância percorrida por um veículo para 0.
// ==============================================
stock bool:ResetVehicleDistance(vehicleid)
{
	return SetVehicleDistance(vehicleid, 0.0); // Chama SetVehicleDistance para resetar
}

// ==============================================
// Função: SetVehicleDistance
// Descrição: Define a distância percorrida por um veículo para um valor específico.
// ==============================================
stock SetVehicleDistance(vehicleid, Float:distance)
{
	if(IsValidVehicle(vehicleid)) // Verifica se o veículo é válido
	{
		a_info[vehicleid][f_distance] = distance; // Define a nova distância
		return true; // Se a distancia for definida retorna true
	}
	return false; // Se a distancia nao for definida retorna false
}

// ==============================================
// Função: IsVehicleBeingTracked
// Descrição: Verifica se um veículo está sendo rastreado pelo sistema.
// ==============================================
stock bool:IsVehicleBeingTracked(vehicleid)
{
	for (new i; i < i_vehicles; i++)
		if(a_vehicle[i] == vehicleid) return true; // Se o veículo está na lista, retorna verdadeiro

	return false; // Se não está na lista, retorna falso
}

// ==============================================
// Função: DisableVehicleTracking
// Descrição: Desabilita o rastreamento de um veículo.
// ==============================================
stock bool:DisableVehicleTracking(vehicleid)
{
	new bool:removed;
	if(IsValidVehicle(vehicleid)) // Verifica se o veículo é válido
	{
		// Loop para remover o veículo da lista
		for(new i; i < i_vehicles; i++)
		{
			if(removed) a_vehicle[i] = a_vehicle[i + 1]; // Reorganiza a lista após remoção
			else if(a_vehicle[i] == vehicleid) // Se encontrar o veículo
			{
				a_vehicle[i] = a_vehicle[i + 1]; // Remove o veículo
				removed = true;
			}
		}
		
		if(removed) i_vehicles--; // Diminui o número de veículos se foi removido
	}
	return removed; // Se o veiculo foi removido retorna true, senao retorna false
}

// ==============================================
// Função: EnableVehicleTracking
// Descrição: Habilita o rastreamento de um veículo.
// ==============================================
stock bool:EnableVehicleTracking(vehicleid)
{
	// Verifica se o veículo é válido e se ainda não está sendo rastreado
	if(IsValidVehicle(vehicleid) && !IsVehicleBeingTracked(vehicleid))
	{
		if(i_vehicles < MAX_VEHICLES) // Verifica se o limite de veículos não foi atingido
		{
			a_vehicle[i_vehicles++] = vehicleid; // Adiciona o veículo à lista

			// Se o timer não estiver ativo, o inicia
			if(!i_timer)
				i_timer = SetTimer("UpdateVehicleDistance", ODSYS_FRAMERATE, true);

			return true; // Se o veiculo foi adicionado retorna true
		}
	}
	return false; // Se o veiculo nao foi adicionado retorna false
}