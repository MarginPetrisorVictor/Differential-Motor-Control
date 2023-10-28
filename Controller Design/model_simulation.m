% Interfata pentru identificare:
% 1. Comunicare seriala de la arduino in matlab (turatie, comanda, timp)
% 2. Salvarea datelor pentru identificare.
% 3. Afisarea pe un plot a datelor
% 4. Buton de Start/Stop communication
% 5. Resetarea datelor, se reimprospateaza vectorul care retine datele.


% Se importeaza datele (viteza masurata in rad/s si factorul de umplere)
% din fisierul Date_Identificare;

viteza = DateIdentificare(:,1)';
u = DateIdentificare(:,2)';
t = 0:1:4858;

viteza_treapta = viteza(1,1:858);
u_treapta = u(1,1:858);
t = 0:857;

viteza_treapta = medfilt1(viteza_treapta,85);
u_treapta = medfilt1(u_treapta,85);

plot(t,viteza_treapta,t,u_treapta*6500);

T = 47.5/1000;
Tm = 35/1000;
K = 862.89;

t = 0:4857;

H = tf(K,[T 1],'IODelay', Tm);
u = u(1,1:4858);
viteza = viteza(1,1:4858);
system = lsim(H,viteza,t);

plot(t,system,t,viteza);
%date_identificare = iddata(u',viteza',0.01);
%model = oe(date_identificare,[1 1 1]);