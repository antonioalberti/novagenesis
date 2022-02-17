% Clear scope
clear;

% Simulation of Interaction
Interaction = 1000;

% Max bytes per webpages
MaxBytesWebPages = 200;

% Max bytes for each object web
MaxBytesObjectWeb = 2000;

% Probabilities
% - Access the same webpage
ProbSameWebPage = 0.8;
% - Web object repeated
ProbRepeated = 0.8;

% Inicialization - Algorithm

% Number of webpages to simulate
NumWebPages = Interaction;
% Variables to save that web pages can 
% access or have accessed
WebPagesToAccess = zeros(NumWebPages,1);
WebPagesAccessed = zeros(NumWebPages,1);

% Variables to plot
% Bytes Transfered
BytesTransferedAW = zeros(Interaction,1);
BytesTransferedNG = zeros(Interaction,1);
% Bytes used from each cache
BytesFromCacheAW = zeros(Interaction,1);
BytesFromCacheNG = zeros(Interaction,1);
% Bytes used in simulation
BytesUsed = zeros(Interaction,1);

% Bytes Transfered during the webpage access
% - For actual web
AWBytesTransfered = 0;
% - For NovaGenesis Web
NGBytesTransfered = 0;

% Total Bytes Used from Cache
TotalBytesUsedInAWCache = 0;
TotalBytesUsedInNGCache = 0;
% Total Bytes Used
TotalBytesUsed = 0;

% Generation of webpages data
% RANDI - Uniformly distributed pseudorandom integers
WebPagesData = randi(MaxBytesWebPages,NumWebPages,1);

% For each webpage:
for i = 1:NumWebPages
    % - Assign a repeated object
    WebPagesData(i,2) = randi(MaxBytesObjectWeb);
    % - Set index to provide a webpage can be accessed
    WebPagesToAccess(i,1) = i;
end

% Size of webpages accessed
ka_size = 1;

% First Interaction of Simulation
% Get random webpage to access and Set it to accessed
kta = randi(size(WebPagesToAccess,1));
k = WebPagesToAccess(kta);
WebPagesAccessed(ka_size) = k;
ka_size = ka_size + 1;
WebPagesToAccess(kta,:) = [];
% Total Bytes Transfered by first interaction
BytesToTransfer = WebPagesData(k,1) + WebPagesData(k,2);
AWBytesTransfered = AWBytesTransfered + BytesToTransfer;
NGBytesTransfered = NGBytesTransfered + BytesToTransfer;
TotalBytesUsed = TotalBytesUsed + BytesToTransfer;
% To Plot
BytesTransferedAW(1) = AWBytesTransfered;
BytesTransferedNG(1) = NGBytesTransfered;
BytesFromCacheAW(1) = TotalBytesUsedInAWCache;
BytesFromCacheNG(1) = TotalBytesUsedInNGCache;
BytesUsed(1) = TotalBytesUsed;

% Simulation for the rest of the interactions
for i = 1:Interaction-1
    UserAccess = rand();
    if( UserAccess <= ProbSameWebPage )
        % Access same webpage
        ka = randi(ka_size-1);
        k = WebPagesAccessed(ka,1);
        BytesInCache = WebPagesData(k,1) + WebPagesData(k,2);
        TotalBytesUsedInAWCache = TotalBytesUsedInAWCache + BytesInCache;
        TotalBytesUsedInNGCache = TotalBytesUsedInNGCache + BytesInCache;
        TotalBytesUsed = TotalBytesUsed + BytesInCache;
    else
        % Get random webpage to access and Set it to accessed
        kta = randi(size(WebPagesToAccess,1));
        k = WebPagesToAccess(kta);
        WebPagesAccessed(ka_size) = k;
        ka_size = ka_size + 1;
        WebPagesToAccess(kta,:) = [];
        
        ObjectRepeated = rand();
        if( ObjectRepeated <= ProbRepeated )
            % Accessing other webpage with objects repeated
            BytesToTransfer = WebPagesData(k,1) + WebPagesData(k,2);
            AWBytesTransfered = AWBytesTransfered + BytesToTransfer;
            TotalBytesUsed = TotalBytesUsed + BytesToTransfer;
            BytesToTransfer = WebPagesData(k,1);
            BytesInCache = WebPagesData(k,2);
            NGBytesTransfered = NGBytesTransfered + BytesToTransfer;
            TotalBytesUsedInNGCache = TotalBytesUsedInNGCache + BytesInCache;
        else
            % Accessing other webpage without objects repeated
            BytesToTransfer = WebPagesData(k,1) + WebPagesData(k,2);
            AWBytesTransfered = AWBytesTransfered + BytesToTransfer;
            NGBytesTransfered = NGBytesTransfered + BytesToTransfer;
            TotalBytesUsed = TotalBytesUsed + BytesToTransfer;
        end
    end
    % Saving to plot
    BytesTransferedAW(i+1) = AWBytesTransfered;
    BytesTransferedNG(i+1) = NGBytesTransfered;
    BytesFromCacheAW(i+1) = TotalBytesUsedInAWCache;
    BytesFromCacheNG(i+1) = TotalBytesUsedInNGCache;
    BytesUsed(i+1) = TotalBytesUsed;
end

%Plot
disp('Report: ')
disp(['- Interaction: ' num2str(Interaction)])
disp('- Bytes Transfered for:');
disp(['   Actual Web: ' num2str(AWBytesTransfered)])
disp(['   NovaGenesis Web: ' num2str(NGBytesTransfered)])
disp('- Bytes Used from:');
disp(['   AWCache: ' num2str(TotalBytesUsedInAWCache)])
disp(['   NGBrowser: ' num2str(TotalBytesUsedInNGCache)])
disp(['- Total Bytes Used: ' num2str(TotalBytesUsed)])
%disp('- Access Sequence of the Sites: ')
%disp(WebPagesAccessed);

% Set range and offset data
range = 0:1:Interaction;
BytesTransferedAW = [0;BytesTransferedAW];
BytesTransferedNG = [0;BytesTransferedNG];
BytesUsed = [0;BytesUsed];

plot(range,BytesTransferedAW,'r',range,BytesTransferedNG,'b',range,BytesUsed,'g');
%plot(range,BytesFromCacheAW,'b',range,BytesFromCacheNG,'r',range,BytesUsed,'g');













