import numpy as np
import random
aver = 0

def f(x):
    global aver
    return (100 - (aver + x[0]))**2
def pso_opt(function, data, e=0.0005, lr_reduce=True, minimum=True, l_sol=0, u_sol=12, n_particles=12, iteration=12, n_dim=1, c1=0.3, c2=0.4, w=0.9, l_teta=0.4, u_teta =0.9, dif_bound=False, dec_lr=False, verbose=[], perhitungan=[]) :
    #print(iteration)
    #inisialisasi posisi dan kecepatan particle awal
    particles_pos = np.zeros((n_particles, n_dim))
    particles_vel = np.zeros((n_particles, n_dim))
    # print(particles_pos)
    # print(particles_vel)
    if dif_bound :
        l_sol=[]
        u_sol=[]
        for i in range(n_dim):
            #print('Masukkan nilai batas bawah solusi dimensi ke-', i+1)
            x = input()
            x = float(x)
            l_sol.append(x)
            #print('Masukkan nilai batas atas solusi dimensi ke-', i+1)
            y = input()
            y = float(y)
            u_sol.append(y)
            for j in range(n_particles):
                particles_pos[j][i] = random.uniform(x, y)
                particles_vel[j][i] = random.uniform(-abs(y-x), abs(y-x))
    else :
        for i in range(n_dim):
            for j in range(n_particles):
                particles_pos[j][i] = data[j][i]
                particles_vel[j][i] = particles_vel[j][i]
            
        # Note : Ini ditampilin di menu parameter
        
        # print(particles_pos)
        # ditampilin di bagian B
        # print(particles_vel)
        # ditampilin di bagian A
    
    #Menentukan Local Best dan Global Best awal
    #LOCAL BEST
    local_value=[]
    local_best=[]
    for i in particles_pos:
        value = function(i) # diambil dari def f(x)
        local_value.append(value)
        local_best.append(i)
    # print('local_value', local_value)
    # print('local_best', local_best)
    #GLOBAL BEST
    best_value = min(local_value)
    global_best = local_best[local_value.index(min(local_value))]
    # print('global_value', best_value)
    # print('global_best', global_best)

    #Update Kecepatan dan posisi particle
    it = 0
    stop = False
    # print(len(particles_pos))
    temp_perhitungan_best = []
    temp_perhitungan_value = []
    while(it < iteration):
        #print(it)
        temp_verbose = [ ]
        
        if stop:
            break
        else:
            if lr_reduce:
                w = u_teta - ((u_teta - l_teta)/iteration)*(it + 1)
            for i in range(len(particles_pos)):
                # print(len(particles_pos))
                new_particles_pos = np.zeros((1, n_dim))[0]
                # print(new_particles_pos)
                for j in range(len(particles_pos[i])):
                    # print(len(particles_pos[i]))
                    r1 = random.uniform(0, 1)
                    r2 = random.uniform(0, 1)
                    v_new = w*particles_vel[i][j] + c1*r1*(local_best[i][j]-particles_pos[i][j]) + c2*r2*(global_best-particles_pos[i][j])
                    p_new = v_new + particles_pos[i][j]
                    new_particles_pos[j] = p_new[0]
                    particles_vel[i][j] = v_new[0]
                    #print(new_particles_pos) #Ini yang dijadikan sebagai scatter bang
                    temp_verbose.append(new_particles_pos[0])
                    #Update Local Best
                if minimum:
                    if function(new_particles_pos) < function(local_best[i]):
                        local_best[i] = new_particles_pos
                        local_value[i] = function(new_particles_pos)
                else:
                    if function(new_particles_pos) > function(local_best[i]):
                        local_best[i] = new_particles_pos
                        local_value[i] = function(new_particles_pos)
                temp_perhitungan_best.append([c.tolist() for c in local_best])
                temp_perhitungan_value.append([c.tolist() for c in local_value])
            #print('local_best', local_best)
            #print('local_value', local_value)
            #Update Global Best
            if minimum:
                best_value = min(local_value)
                global_best = local_best[local_value.index(min(local_value))]
            else:
                best_value = max(local_value)
                global_best = local_best[local_value.index(max(local_value))]
        #print('global_best', global_best)
        #print('value', best_value)
        # print('nilai w', w)
        # Dicari konvergensinya, dari nilai local_value
        if best_value < e:
            stop = True
        it += 1
        perhitungan.append([temp_perhitungan_best,temp_perhitungan_value, [c.tolist() for c in local_best], global_best.tolist(),best_value])
        temp_perhitungan_best = []
        temp_perhitungan_value = []
        verbose.append(temp_verbose)
    #print(it)
    #print('Global Best', global_best)
    # print('Global Best Value', best_value)
    return global_best.tolist(), best_value.tolist(), verbose, perhitungan
def calculate(data):
    global aver
    aver = np.average(data)
    verbose = []
    return pso_opt(f, data, iteration=20, n_particles=12, c1=0.3, c2=0.5, lr_reduce=True, verbose=verbose, perhitungan=[])
if __name__ == "__main__":
    data = [[43.55], 
        [44.33], 
        [52.25], 
        [47.86], 
        [29.45], 
        [52.42], 
        [31.56], 
        [57.27], 
        [45.32], 
        [37.98], 
        [52.83], 
        [64.85]]
    print(calculate(data))