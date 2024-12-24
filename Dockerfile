FROM devkitarm
# Copy the source code
COPY project project
WORKDIR "/project"
RUN make
