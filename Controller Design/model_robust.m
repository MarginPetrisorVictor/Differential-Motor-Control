T = 0.8;
Knom = 2225;
dK = ureal('dK',0,'Range',[-225 225]);

Hdk = tf(dK);

G0 = tf(Knom,[T 1]);
G = G0 + Hdk;
G.InputName = "u";
G.OutputName = "y1";

Ws = makeweight(100,[1 0.5],0.25);
Ws.InputName = "y";
Ws.OutputName = "e";

Sum = sumblk('y = y1 + d');

inputs = {'d','u'};
outputs = {'e','y'};

P = connect(G,Ws,Sum,inputs,outputs);

C0 = tunableSS('K',1,1,1);
CL0 = lft(P,C0);

[CL,CLperf,info] = musyn(CL0);

u = TreaptaIdentificare(:,1)';
t = 0:0.1:75.3;
lsim(CL*3.5,u,t);