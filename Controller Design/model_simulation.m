% Interfata pentru identificare:
% 1. Comunicare seriala de la arduino in matlab (turatie, comanda, timp)
% 2. Salvarea datelor pentru identificare.
% 3. Afisarea pe un plot a datelor
% 4. Buton de Start/Stop communication
% 5. Resetarea datelor, se reimprospateaza vectorul care retine datele.


% Se importeaza datele (viteza masurata in rad/s si factorul de umplere)
% din fisierul Date_Identificare;

v_id = TreaptaIdentificare(:,1)';
u_id = TreaptaIdentificare(:,2)';
t_id = 0:0.1:75.3;
v_v = TreaptaVerificare(:,1)';
u_v = TreaptaVerificare(:,2)';
t_v = 0:0.1:74.9;
Ts = 0.1;

plot(t_id,u_id*200,t_id,v_id);

dateIdentificare = iddata(v_id',u_id',Ts);
dateValidare = iddata(v_v',u_v',Ts);

time = seconds(t_v');
dataInput = timetable(time,u_v');
dataOut = timetable(time,v_v');

A = -1/0.8;
B = 1/0.8;
R = 1;
Q = 7;
K = lqr(A,B,Q,R);