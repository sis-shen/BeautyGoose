FROM nginx:1.24.0
# 先删除默认配置
RUN rm /etc/nginx/conf.d/default.conf
COPY ./sup.conf /etc/nginx/conf.d
CMD ["nginx","-g","daemon off;"]
ENTRYPOINT [ "/docker-entrypoint.sh" ]