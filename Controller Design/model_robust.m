T = 0.8;
Knom = 2225;

u = 0.05:0.01:0.2;
k = [2060 1883.33 1742.86 1637.5 1533.33 1460 1390.91 1333.33 1284.62 1235.71 1186.67 1156.25 1123.53 1094.44 1057.89 1045];
v = [103 113 122 131 138 146 153 160 167 174 178 185 191 197 201 205];

k = k + 50;
% figure
% plot(u,k);
% title('K = f(u)')
% xlabel('Comanda[%]')
% ylabel('Gain')
% figure
% plot(v,k);
% title('K = f(viteza)')
% xlabel('Viteza[RPS]')
% ylabel('Gain')

%% Fitted model pentru K = f(v);
p1 = 0.06328;
p2 = -29.11;
p3 = 4417;
expression = @(x)(p1*x^2 + p2*x + p3);

data = ones(1,length(k));

for i = 1:16
    data(i) = expression(v(i));
end

figure
plot(data), hold on
plot(k)

%% Fitted model pentru K = f(u);
p1 = 2.587e+06;
p2 = -1.592e+06;
p3 = 3.832e+05;
p4 = -4.647e+04;
p5 = 3656;
expression = @(x)(p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5);

data = ones(1,length(k));

for i = 1:16
    data(i) = expression(u(i));
end

figure
plot(data), hold on
plot(k)

